#ifndef INC_SemanticException_hpp__
#define INC_SemanticException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: SemanticException.hpp 1361 2007-06-07 02:34:05Z vkurland $
 */

#include <antlr/config.hpp>
#include <antlr/RecognitionException.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API SemanticException : public RecognitionException {
public:
	SemanticException(const ANTLR_USE_NAMESPACE(std)string& s)
	: RecognitionException(s)
	{
	}
	SemanticException(const ANTLR_USE_NAMESPACE(std)string& s,
							const ANTLR_USE_NAMESPACE(std)string& fileName_,
							int line_,int column_)
	: RecognitionException(s,fileName_,line_,column_)
	{
	}

	~SemanticException() throw()
	{
	}
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_SemanticException_hpp__
