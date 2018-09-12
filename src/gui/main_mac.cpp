/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: main.cpp 493 2008-08-30 05:05:56Z vadim $


  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/


#include "global.h"

#include <QDir>
#include <QString>

#include "FWWindow.h"
#include "FWBSettings.h"

using namespace std;

#if defined(Q_WS_MAC)
#include <Carbon/Carbon.h>

static OSErr checkAppleEventForMissingParams(const AppleEvent&  
                                             theAppleEvent)
{
    DescType returnedType;
    Size actualSize;
    OSErr err;

    switch (err = AEGetAttributePtr(&theAppleEvent,  
                                    keyMissedKeywordAttr,
                                    typeWildCard,
                                    &returnedType,
                                    nil,
                                    0,  
                                    &actualSize))
    {
    case errAEDescNotFound:
        return noErr;
    case noErr:
        return errAEEventNotHandled;
    default:
        return err;
    }
}

static pascal OSErr odocHandler(const AppleEvent* inEvent, AppleEvent*  
                                /*reply*/, SRefCon /*refCon*/)
{
    if (fwbdebug) qDebug("Handling 'odoc'\n");
    AEDescList documentList;
    OSErr err = AEGetParamDesc(inEvent, keyDirectObject, typeAEList,  
                               &documentList);
    if (err == noErr)
    {
        err = checkAppleEventForMissingParams(*inEvent);

        if (err == noErr)
        {
            long documentCount;
            err = AECountItems(&documentList, &documentCount);

            for (long documentIndex = 1; err == noErr && documentIndex <=  
                     documentCount; documentIndex++)
            {
                // What kind of document is it?
                DescType returnedType;
                Size actualSize;
                err = AESizeOfNthItem(&documentList,
                                      documentIndex,
                                      &returnedType,  
                                      &actualSize);
                if (err == noErr)
                {
                    // It's just a normal document file
                    AEKeyword keyword;
                    FSRef ref;
                    err = AEGetNthPtr(&documentList,
                                      documentIndex,
                                      typeFSRef,  
                                      &keyword,
                                      &returnedType,
                                      (Ptr)&ref, sizeof(FSRef), &actualSize);

                    if (err == noErr)
                    {
                        char buf[1024];
                        FSRefMakePath(&ref, reinterpret_cast<UInt8*>(buf),1024);

                        QDir file(buf);
                        mw->registerAutoOpenDocFile(file.canonicalPath(),
                                                    false);
                        //mw->openDocFiles << file.canonicalPath();
                    }
                }
            }
        }
        AEDisposeDesc(&documentList);
    }
    if  (fwbdebug) qDebug("Returning %d from handleOpenDocuments\n", err);
    return err;
}

void connectOdocHandler()
{
    AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,  
                          NewAEEventHandlerUPP(odocHandler),0,false);

}

#endif  // Q_WS_MAC

