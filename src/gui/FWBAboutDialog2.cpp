/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$

*/


#include "../../config.h"
#include "global.h"
#include "FWBAboutDialog.h"

/**
 * Do co-branding here. Set text in the label object branding to
 * whatever co-branding string there should be.
 *
 * Also this is where we add a label to indicate that the program is
 * registered using label object reg.
 *
 * Labels in the dialog are as follows:
 *
 *    branding
 *    copyright
 *    web_site_url
 *    reg
 * 
 * All these are regular QLabel objects, use setText() to set or change text.
 * QLabel copyright by default shows text "Copyright 2002-2009  NetCitadel, LLC"
 */
void FWBAboutDialog::fillTheBlanks() 
{
    m_aboutDialog->branding->setText("");

#ifdef ELC
    switch (registered)
    {
    case 1:
        m_aboutDialog->reg->setText(tr("Invalid license"));
        break;
    case 2:
        m_aboutDialog->reg->setText(tr("Registered"));
        break;
    default:
        m_aboutDialog->reg->setText(tr("Unregistered"));
        break;
    }            
#endif
};

