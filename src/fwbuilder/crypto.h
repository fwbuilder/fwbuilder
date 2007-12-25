/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Zaliva <lord@crocodile.org>

  $Id: crypto.h 966 2006-08-18 03:59:32Z vkurland $

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

#ifndef __CRYPTO_HH_FLAG__
#define __CRYPTO_HH_FLAG__


#include <fwbuilder/libfwbuilder-config.h>

#include <vector>

#ifdef HAVE_LIBSSL
# include <openssl/ssl.h>
# include <openssl/err.h>
# include <openssl/pkcs12.h>
#else
/* dummy definitions to compile when libssl not present */
  #undef EVP_PKEY
  #undef X509_NAME
  #undef X509
  typedef void EVP_PKEY;
  typedef void X509_NAME;
  typedef void X509;

# define MBSTRING_ASC 0

# define NID_description            0
# define NID_countryName            0
# define NID_stateOrProvinceName    0
# define NID_localityName           0
# define NID_organizationName       0
# define NID_organizationalUnitName 0
# define NID_commonName             0
# define NID_pkcs9_emailAddress     0
#endif

#include <fwbuilder/FWException.h>

namespace libfwbuilder
{

    void init_ssl();

    /**
     * Decode BASE64 encoded, NULL terminated string 'in'.
     * are puts results into 'out'. Returns length of -1
     * if error occured.
     */
    int unbase64(const char *in, char **out) throw(libfwbuilder::FWException);


    class KeyGenerationCallback
    {
        public:
        virtual ~KeyGenerationCallback() {}
        /**
         * This callback will be called when RSA key is generated.
         * See RSA_generate_key(3) for parameters description.
         */
        virtual void progress(int, int) = 0;
    };

    /**
     * Public or private key.
     */
    class Key
    {
        public:

        /**
         * Creates key from EVP_PKEY* object.
         * (copy is made)
         */
        Key(EVP_PKEY *, bool _is_private=false) throw(FWException);

        /**
         * Copy constructor.
         */
        Key(const Key &k) throw(FWException);
        
        /**
         * Generates new key. 
         * Default key type is RSA and length 1024 bits.
         *
         * If callback is not NULL it will be called during 
         * key generation.
         */
        Key(int bits=1024, KeyGenerationCallback *callback=NULL) throw(FWException);

        /**
         * Creates key from external PEM representation.
         */
        Key(const std::string &, bool _is_private=false, const std::string *pass=NULL) throw(FWException);

        virtual ~Key();

        bool isPrivate() const;

        /**
         * Returns string representation of Public key This likely
         * would be unencrypte PEM formatted string, which could
         * contain linefeeds.
         */
        std::string getPublicKey() const throw(FWException);

        /**
         * Returns string representation of Private key This likely
         * would be 3DES encrypted PKCS#8 formatted string, which
         * could contain linefeeds.
         *
         * @param pass Passphrase used to encrypt private key
         */
        std::string getPrivateKey(const std::string &pass) const throw(FWException);

        /**
         * Returns 'fingerprint' of public key component.
         */
        std::string getFingerprint() const throw(FWException);

        operator EVP_PKEY*() const;
        EVP_PKEY* getInternal() const;

        protected:

        EVP_PKEY* pkey_clone(EVP_PKEY *, bool is_private) throw(FWException);

        private:
 
        bool is_private;
        EVP_PKEY *key;
    };

    class X509_entry
    {
        public:
        
        X509_entry(const X509_entry &);

        X509_entry(int nid,
                   const std::string &value,
                   int len  = -1,
                   int loc  = -1,
                   int set  =  0
        );

        X509_entry(const std::string &id,
                   const unsigned char *bytes,
                   int type = MBSTRING_ASC,
                   int len  = -1,
                   int loc  = -1,
                   int set  =  0
        );

        X509_entry(int nid,
                   const unsigned char *bytes,
                   int type = MBSTRING_ASC,
                   int len  = -1,
                   int loc  = -1,
                   int set  =  0
        );
                   
        virtual ~X509_entry();

        /**
         * adds to given name.
         */
        void addTo(X509_NAME *name) const throw(FWException);

        private:

        /**
         * Type of ID used, numeric or string.
         */
        enum
        {
            by_NID, // numeric ID used
            by_TXT  // string ID used
        } id_type;
        
        /**
         * Numeric ID.
         * e.g. NID_countryName
         */
        int   nid;

        /**
         * String ID
         * e.g. "CN"
         */
        char *id;

        /**
         * Type
         * e.g. MBSTRING_ASC
         */
        int type;

        /**
         * Pointer to data.
         */
        unsigned char *bytes;

        int len; 
        int loc; 
        int set;
    };
    
    /**
     * X509 Certificate.
     */
    class Certificate
    {
        public:
        
        /**
         * Generates new self signed certificate. 
         *
         * @param private_key key used to sign certificate
         * @param entries list of entries to be added to certificate.
         * @param not_before will be valid not before given time offset (in seconds from
         *        current time).
         * @param not_after will be valid not after given time offset (in seconds from
         *        current time).
         */
        Certificate(Key *private_key, 
                    const std::vector<X509_entry> &entries,
                    long not_before = 0l,
                    long not_after  = long(60*60*24*365)
        ) throw(FWException);

        Certificate(const std::string &) throw(FWException);

        Certificate(X509 *cert) throw(FWException);

        Certificate(const Certificate &) throw(FWException);

        virtual ~Certificate();

        /**
         * Returns string representation of this certificate.
         * This likely would be PEM formatted string, which
         * could contain linefeeds.
         */
        operator std::string() const;

        /**
         * Returns public key associated with this certificate.
         */
        const Key *getPublicKey() const;

        /**
         * Return OpenSSL X509 object.
         */
        operator X509*() const;
        X509* getInternal() const;

        protected:

        X509* cert_clone(X509 *) throw(FWException);
        
        private:
        
        Key  *key;
        X509 *cert;
    };
    
    /**
     * Key/Cert pairs management, including
     * PKCS12 import/export.
     * 
     * @see http://www.rsasecurity.com/rsalabs/pkcs/pkcs-12/
     */
    class KeyAndCert
    {
        public:

        /**
         * Copy constructor
         */
        KeyAndCert(const KeyAndCert &) throw(FWException);

        /**
         * Creates new object from given Key and Certificate.
         */
        KeyAndCert(const Key &, const Certificate &) throw(FWException);
        
        /**
         * Constructs new object from PKCS12 data (binary).
         */
        KeyAndCert(const char *data, size_t len, const std::string &pass) throw(FWException);
        
        virtual ~KeyAndCert();

        /**
         * Return key/cert pair as PKCS12 encoded string.
         *
         * @param pass password
         * @param name friendly name (optional)
         */
        std::string asString(const std::string &pass, const std::string *name=NULL) const throw(FWException);

        const Key &getPrivateKey() const;

        const Certificate &getCertificate() const;
        
        private:

        Key         *key;
        Certificate *cert;
        
    };

};

#endif // __CRYPTO_HH_FLAG__
