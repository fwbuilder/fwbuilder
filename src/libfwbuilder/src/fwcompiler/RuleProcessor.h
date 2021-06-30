/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Zaliva  <lord@crocodile.org> ,  Vadim Kurland <vadim@fwbuilder.org>

  $Id$

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

#ifndef __RULE_PROCESSOR_HH__
#define __RULE_PROCESSOR_HH__

#include <deque>

#include "fwbuilder/Rule.h"


namespace fwcompiler 
{

    class Compiler;

    /**
     * This class represents Rule processing chain elements. It
     * takes input from other processor and produces modified output
     * via getNextRule() method.
     *
     * For cases when one input rule produce several output rules
     * internal temporary queue is provided.
     *
     * This is abstract class. Implementator should subclass it
     * and implement processNext() method.
     *
     * @author Vadim Zaliva <lord@crocodile.org>, Vadim Kurland <vadim@fwbuilder.org>
     */

    class BasicRuleProcessor
    {
        public:
        
        BasicRuleProcessor()
        {
            compiler=nullptr;
            prev_processor=nullptr;
            name="";
            do_once=false;
        }

        BasicRuleProcessor(const std::string &_name)
        {
            compiler=nullptr;
            prev_processor=nullptr;
            name=_name;
            do_once=false;
        }

        virtual ~BasicRuleProcessor()
        {
            // Delete unused elements
            while(!tmp_queue.empty())
            {
//                libfwbuilder::Rule *x = tmp_queue.front();
                tmp_queue.pop_front();
//                delete x;
            }
        }

        /**
         * Ties this rule processor to the compiler. Parameter _comp
         * is policy compiler object that is using this processor.
         */
        void setContext(Compiler *_comp) { compiler=_comp; }

        /**
         * Connects this rule processor to the previous in the chain.
         * Parameter _src is source processor which will be used to
         * obtain input from.
         */
        void setDataSource(BasicRuleProcessor *_src) { prev_processor=_src; }

        /**
         * sets name for this rule processor. Name is used in debug
         * print
         */
        void  setName(const std::string &_name) { name=_name; }

        /**
         * returns name of this processor
         */
        std::string getName() { return name; }

        /**
         * Returns next rule or nullptr if no more is availiable.
         */
        libfwbuilder::Rule *getNextRule()
        {
            while(tmp_queue.empty() && processNext()) ;

            if(tmp_queue.empty())
            {
                return nullptr;
            } else
            {
                libfwbuilder::Rule *res = tmp_queue.front();
                tmp_queue.pop_front();
                return res;
            }
        }

        /**
         *  some processors work on the whole rule set rather than on
         *  a single rule. This method reads input data stream while
         *  rules are coming from the previous rule processor and fill
         *  tmp_queue, then return true if there are rules in the
         *  queue and false otherwise. Using this instead of getNext
         *  in processNext in rule processors that need the whole rule
         *  set. This method executes only once and returns false upon
         *  subsequent calls right away.
         */
        bool slurp()
        {
            if (!do_once) {
                libfwbuilder::Rule *rule;
                while ( (rule=prev_processor->getNextRule())!=nullptr) tmp_queue.push_back(rule);
                do_once=true;
                return (tmp_queue.size()!=0);
            }
            return false;
        }

        /**
         * Implementor should implement this method in a manner it
         * process one or more source elements and adds results to tmp
         * queue. It is prefferable to process just one source element
         * at a time.
         *
         * @return false if no more elements could be produced.
         */
        virtual bool processNext() = 0;
        
        protected:

        std::string                        name;
        std::deque<libfwbuilder::Rule *>   tmp_queue;
        BasicRuleProcessor                *prev_processor;
        Compiler                          *compiler;
        bool                               do_once;
    };

    /**
     *  This class provides convenient interface by adding
     *  dynamic_cast so that pointer to the rule returned from getNext
     *  would have type PolicyRule*
     */
    class PolicyRuleProcessor : public BasicRuleProcessor
    {
        public:
        PolicyRuleProcessor() : BasicRuleProcessor() {}
        PolicyRuleProcessor(const std::string &_name) : BasicRuleProcessor(_name) {}
        /**
         * Returns next rule to be processed or nullptr if no more is availiable.
         */
        virtual libfwbuilder::PolicyRule* getNext()
        {
            return dynamic_cast<libfwbuilder::PolicyRule*>( prev_processor->getNextRule() );
        }
    };


    /**
     *  This class provides convenient interface by adding
     *  dynamic_cast so that pointer to the rule returned from getNext
     *  would have type NATRule*
     */
    class NATRuleProcessor : public BasicRuleProcessor
    {
        public:
        NATRuleProcessor() : BasicRuleProcessor() {}
        NATRuleProcessor(const std::string &_name) : BasicRuleProcessor(_name) {}
        /**
         * Returns next rule to be processed or nullptr if no more is availiable.
         */
        virtual libfwbuilder::NATRule* getNext()
        {
            return dynamic_cast<libfwbuilder::NATRule*>( prev_processor->getNextRule() );
        }
    };

    
     /**
     *  This class provides convenient interface by adding
     *  dynamic_cast so that pointer to the rule returned from getNext
     *  would have type RoutingRule*
     */
    class RoutingRuleProcessor : public BasicRuleProcessor
    {
        public:
        RoutingRuleProcessor() : BasicRuleProcessor() {}
        RoutingRuleProcessor(const std::string &_name) : BasicRuleProcessor(_name) {}
        /**
         * Returns next rule to be processed or nullptr if no more is availiable.
         */
        virtual libfwbuilder::RoutingRule* getNext()
        {
            return dynamic_cast<libfwbuilder::RoutingRule*>( prev_processor->getNextRule() );
        }
    };


};

#endif // __RULE_PROCESSOR_HH__
