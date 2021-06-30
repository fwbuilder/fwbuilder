/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id$
 */
#include "antlr/ASTRefCount.hpp"
#include "antlr/AST.hpp"

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

ASTRef::ASTRef(AST* p)
: ptr(p), count(1)
{
	if (p && !p->ref)
		p->ref = this;
}

ASTRef::~ASTRef()
{
	delete ptr;
}

ASTRef* ASTRef::getRef(const AST* p)
{
	if (p) {
		AST* pp = const_cast<AST*>(p);
		if (pp->ref)
			return pp->ref->increment();
		else
			return new ASTRef(pp);
	} else
		return nullptr;
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

