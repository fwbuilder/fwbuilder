/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Zaliva <lord@crocodile.org>

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

#include <assert.h>

#include <fwbuilder/crypto.h>
#include <fwbuilder/Tools.h>

using namespace libfwbuilder;
using namespace std;

// #ifndef HAVE_LIBSSL
// # warning "openssl library not present - all crypto code is disabled."
// #endif

void libfwbuilder::init_ssl()
{
#ifdef HAVE_LIBSSL
    /* Initialize OpenSSL library */
    OpenSSL_add_all_algorithms();
    SSL_library_init();
    SSL_load_error_strings();
#endif
}

/*
 * This is quick and dirty implementation using OpenSSL BIO
 * API.
 */
int libfwbuilder::unbase64(const char *in, char **out) throw(libfwbuilder::FWException)
{
#ifdef HAVE_LIBSSL
    char *ncin=cxx_strdup(in);
    
    BIO *bio_out = BIO_new(BIO_s_mem());
    BIO *bio = BIO_push(BIO_new(BIO_f_base64()), BIO_new_mem_buf(ncin, -1));
    
    char inbuf[256];
    int  inlen;
    
    while((inlen = BIO_read(bio, inbuf, sizeof(inbuf))) > 0)
        BIO_write(bio_out, inbuf, inlen);

    BIO_flush(bio_out);
    
    char   *ptr;
    size_t l = BIO_get_mem_data(bio_out, &ptr);
    *out=new char[l];
    memcpy(*out, ptr, l);
    
    BIO_free_all(bio);
    BIO_free_all(bio_out);
    delete[] ncin;
    
    return l;
#else
    throw FWException("libfwbuilder::unbase64: Not implemented");
#endif
}


#ifdef HAVE_LIBSSL

static string last_ssl_err()
{
    char err_buf[1024];
    *err_buf='\0';
    ERR_error_string_n(ERR_get_error(), err_buf, sizeof(err_buf));
    return string(err_buf);
}
#endif

EVP_PKEY* Key::pkey_clone(EVP_PKEY *k, bool is_private) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    if(k->type == EVP_PKEY_RSA) 
    {   
        EVP_PKEY *res = EVP_PKEY_new();
        assert(res!=NULL);
        res->type     = EVP_PKEY_RSA;
        if(is_private)
            res->pkey.rsa=RSAPrivateKey_dup(k->pkey.rsa);
        else
            res->pkey.rsa=RSAPublicKey_dup(k->pkey.rsa);
        
        EVP_PKEY_copy_parameters(k, res);
        return res;
    } else
    {
        //TODO: add support for other key types.
        throw FWException("Unsupported key type. Only RSA keys are currently supported");
    }
#endif
}

Key::Key(const Key &k) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    is_private = k.isPrivate();
    key        = pkey_clone(k.key, is_private);
#endif
}

Key::Key(EVP_PKEY *k, bool _is_private) throw(FWException):
    is_private(_is_private)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    key = pkey_clone(k, is_private);
#endif
}


static void key_generation_progress_callback(int x, int y, void *a)
{
    KeyGenerationCallback *cb=reinterpret_cast<KeyGenerationCallback*>(a);
    cb->progress(x, y);
}

Key::Key(int bits, KeyGenerationCallback *callback) throw(FWException):
    is_private(true) 
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    if(bits<1024)
        throw FWException("Key length under 1024 in insecure and not allowed.");
    
    //TODO: Progress indicator callback
    RSA *rsa_key;

    if(callback)
        rsa_key = RSA_generate_key(bits, RSA_F4, key_generation_progress_callback, callback);
    else
        rsa_key = RSA_generate_key(bits, RSA_F4, NULL, NULL);
        
    if(!rsa_key)
        throw FWException(string("Error generating RSA key: ")+last_ssl_err());
    
    key = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(key, rsa_key);   
#endif
}

Key::Key(const string &s, bool _is_private, const string *pass) throw(FWException):
    is_private(_is_private)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    char *buf=new char[s.length()+1];
    strcpy(buf,s.c_str());
    
    BIO *mem = BIO_new_mem_buf(buf, -1);

    if(_is_private)
        key = PEM_read_bio_PrivateKey(mem, NULL, NULL, (void*)(pass?pass->c_str():NULL));
    else
        key = PEM_read_bio_PUBKEY(mem, NULL, NULL, (void*)(pass?pass->c_str():NULL));

    delete buf;
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free(mem);
    
    if(!key)
        throw FWException("Error decoding key");
#endif
}

Key::~Key()
{
#ifdef HAVE_LIBSSL
    if(key)
        EVP_PKEY_free(key);
#endif
}

bool Key::isPrivate() const
{
    return is_private;
}

std::string Key::getPrivateKey(const string &pass) const throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    if(!is_private)
        throw FWException("Could not get Private key - this is Public key");

    BIO  *mem = BIO_new(BIO_s_mem());

    if(!PEM_write_bio_PKCS8PrivateKey(mem, key, EVP_des_ede3_cbc(), NULL, 0, 0, (void*)pass.c_str()))
    {
        BIO_set_close(mem, BIO_NOCLOSE);
        BIO_free(mem);
        throw FWException("Error exporting public key");
    }
    
    char   *ptr;
    size_t l = BIO_get_mem_data(mem, &ptr);
    char *res=new char[l+1];
    memcpy(res, ptr, l);
    res[l]='\0';
    
    string ext = res;
    delete res;
    
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free(mem);
    return ext;
#endif
}

std::string Key::getPublicKey() const throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    BIO  *mem = BIO_new(BIO_s_mem());
    if(!PEM_write_bio_PUBKEY(mem, key))
    {
        BIO_set_close(mem, BIO_NOCLOSE);
        BIO_free(mem);
        throw FWException("Error exporting public key");
    }
    
    char   *ptr;
    size_t l = BIO_get_mem_data(mem, &ptr);
    char *res=new char[l+1];
    memcpy(res, ptr, l);
    res[l]='\0';
    
    string ext = res;
    delete res;
    
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free(mem);
    return ext;
#endif
}

string Key::getFingerprint() const throw(FWException)
{
    return "TODO"; //TODO: implement
}

Key::operator EVP_PKEY*() const
{
    return key;
}

EVP_PKEY* Key::getInternal() const
{
    return key;
}

X509_entry::X509_entry(const X509_entry &e)
{
    id_type=e.id_type;
    if(id_type==by_TXT)
        id  = cxx_strdup(e.id);
    else
        nid = e.nid;
    type = e.type;
    len = e.len;
    bytes = new unsigned char[len];
    memcpy(bytes, e.bytes, len);
    loc = e.loc;
    set = e.set;
}

X509_entry::X509_entry(int _nid,
                       const string &value,
                       int _len,
                       int _loc,
                       int _set
): id_type(by_NID), nid(_nid), type(MBSTRING_ASC), len(value.length()), loc(_loc), set(_set)
{
    bytes = (unsigned char *)cxx_strdup(value);
}

X509_entry::X509_entry(const string &_id,
                       const unsigned char *_bytes,
                       int _type,
                       int _len,
                       int _loc,
                       int _set
): id_type(by_TXT), type(_type), len(_len), loc(_loc), set(_set)
{
    id    = cxx_strdup(_id);
    bytes = new unsigned char[len];
    memcpy(bytes, _bytes, len);
}

X509_entry::X509_entry(int _nid,
           const unsigned char *_bytes,
           int _type,
           int _len,
           int _loc,
           int _set
): id_type(by_NID), nid(_nid), type(_type), len(_len), loc(_loc), set(_set)
{
    bytes = new unsigned char[len];
    memcpy(bytes, _bytes, len);
}

X509_entry::~X509_entry()
{
    if(id_type==by_TXT)
        delete id;
    delete bytes;
}

void X509_entry::addTo(X509_NAME *name) const throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    int rc;
    if(id_type==X509_entry::by_NID)
        rc = X509_NAME_add_entry_by_NID(name, nid, type, bytes, len, loc, set);
    else
        rc = X509_NAME_add_entry_by_txt(name, id, type, bytes, len, loc, set);
    if(!rc)
    {
        char c[80];
        if(id_type==by_TXT)
            sprintf(c,"Error adding X509 entry with ID='%s'", id);
        else
            sprintf(c,"Error adding X509 entry with NID=%d", nid);
        
        //throw FWException(string(c));
    }
#endif
}


Certificate::Certificate(const Certificate &c) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    if(!c.key || !c.cert)
        throw FWException("Incomplete cerficate");
    key  = new Key(*c.key);
    cert = cert_clone(c.cert);
#endif
}

Certificate::Certificate(Key *private_key, 
                         const std::vector<X509_entry> &entries,
                         long not_before,
                         long not_after
) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    key = new Key(*private_key);
    cert = X509_new();
    X509_set_pubkey(cert, key->getInternal());
    X509_set_version(cert, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(cert), 0L);

    ASN1_TIME_set(X509_get_notBefore(cert), not_before);
    ASN1_TIME_set(X509_get_notAfter(cert), not_after);   
    
    X509_NAME *name = X509_get_subject_name(cert);

    for(std::vector<X509_entry>::const_iterator i=entries.begin();i!=entries.end();++i)
    {
        const X509_entry &e=*i;
        try
        {
            e.addTo(name); 
        } catch(const FWException &ex)
        {
            // TODO: this could throw exeption.
            // free resources.
            throw;
        }
    }

    X509_set_issuer_name(cert, name);
    X509_sign(cert, key->getInternal(), EVP_sha1());
#endif
}

Certificate::Certificate(const string &s) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    char *buf=new char[s.length()+1];
    strcpy(buf,s.c_str());
    
    BIO *mem = BIO_new_mem_buf(buf, -1);
    
    cert = PEM_read_bio_X509(mem, NULL, 0, NULL);
    
    delete buf;
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free(mem);
    
    if(!cert)
        throw FWException("Error decoding certificate");
    
    key = new Key(X509_get_pubkey(cert), false);
#endif
}

Certificate::Certificate(X509 *_cert) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    cert = cert_clone(_cert);
    key = new Key(X509_get_pubkey(cert), false);
#endif
}

Certificate::~Certificate()
{
#ifdef HAVE_LIBSSL
    if(cert)
        X509_free(cert);
#endif
    delete key;
}

Certificate::operator std::string() const
{
#ifndef HAVE_LIBSSL
    return "";
#else
    BIO  *mem = BIO_new(BIO_s_mem());
    if(!PEM_write_bio_X509(mem, cert))
    {
        BIO_set_close(mem, BIO_NOCLOSE);
        BIO_free(mem);
        throw FWException("Error exporting certificate key");
    }
    
    char   *ptr;
    size_t l = BIO_get_mem_data(mem, &ptr);
    char *res=new char[l+1];
    memcpy(res, ptr, l);
    res[l]='\0';
    
    string ext = res;
    delete res;
    
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free(mem);
    return ext;
#endif
}

const Key *Certificate::getPublicKey() const
{
    return key;
}
        
Certificate::operator X509*() const
{
    return cert;
}

X509* Certificate::getInternal() const
{
    return cert;
}

X509* Certificate::cert_clone(X509 *c) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    X509 *res = X509_dup(c);
    if(!res)
        throw FWException(string("Certificate cloning operation failed: ")+last_ssl_err());
    return res;
#endif
}
 
KeyAndCert::KeyAndCert(const Key &k, const Certificate &c) throw(FWException)
{
    key   = new Key(k);
    cert  = new Certificate(c);
}

KeyAndCert::KeyAndCert(const char *s, size_t len, const string &pass) throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    char *buf=new char[len];
    memcpy(buf,s,len);
    BIO *mem = BIO_new_mem_buf(buf, len);

    PKCS12   *pkcs12 = d2i_PKCS12_bio(mem, NULL);
    if(!pkcs12)
    {
        delete buf;
        BIO_set_close(mem, BIO_NOCLOSE);
        BIO_free(mem);
        throw FWException(string("Error decoding PKCS12 data: ")+last_ssl_err());
    }
    
    EVP_PKEY *pk; 
    X509     *pc;
    int rc = PKCS12_parse(pkcs12, pass.c_str(), &pk, &pc, NULL);
    PKCS12_free(pkcs12);
    
    delete buf;
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free(mem);
    
    if(!rc)
        throw FWException("Error parsing PKCS12 data.");
    
    key  = new Key(pk, true);
    cert = new Certificate(pc);
#endif
}

KeyAndCert::KeyAndCert(const KeyAndCert &o) throw(FWException)
{
    key  = new Key(*(o.key));
    cert = new Certificate(*(o.cert));
}
        
KeyAndCert::~KeyAndCert()
{
    delete key  ;
    delete cert ;
}

std::string KeyAndCert::asString(const string &pass, const string *name) const throw(FWException)
{
#ifndef HAVE_LIBSSL
    throw FWException("Compiled without crypto support.");
#else
    PKCS12 *pkcs12;

    char *n = name?cxx_strdup(*name):NULL;
    char *p = cxx_strdup(pass);
    pkcs12 = PKCS12_create(p, n, key->getInternal(), cert->getInternal(),
                           NULL, 0, 0, 0, PKCS12_DEFAULT_ITER, 0);
    delete p;
    delete n;

    if(!pkcs12)
        throw FWException("Error generating PKCS12 data: "+last_ssl_err());

    BIO *mem = BIO_push(BIO_new(BIO_f_base64()), BIO_new(BIO_s_mem()));
    if(!i2d_PKCS12_bio(mem, pkcs12))
    {
        PKCS12_free(pkcs12);
        BIO_set_close(mem, BIO_NOCLOSE);
        BIO_free_all(mem);
        throw FWException("Error exporting PKCS12 data: "+last_ssl_err());
    }
    PKCS12_free(pkcs12);

    BIO_flush(mem);
    
    char   *ptr;
    size_t l = BIO_get_mem_data(mem, &ptr);
    char *res=new char[l+1];
    memcpy(res, ptr, l);
    res[l]='\0';
    
    string ext = res;
    delete res;
    
    BIO_set_close(mem, BIO_NOCLOSE);
    BIO_free_all(mem);
    return ext;
#endif
}

const Key &KeyAndCert::getPrivateKey() const
{
    return *key;
}

const Certificate &KeyAndCert::getCertificate() const
{
    return *cert;
}

