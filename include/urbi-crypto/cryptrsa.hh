/**
 *
 * Copyright (C) Gostai S.A.S., 2006-2008.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information.
 * For comments, bug reports and feedback: http://www.urbiforge.com
 */

#ifndef URBI_CRYPTO_CRYPTRSA_HH
# define URBI_CRYPTO_CRYPTRSA_HH

# include "urbi-crypto/crypt.hh"


namespace crypto
{

  class CryptRSA : public Crypt
  {
    typedef Crypt super_type;

  public:

    CryptRSA ();

    ~CryptRSA () throw ();

    virtual void crypt ();

    virtual void decrypt ();

  };

}

#endif // !URBI_CRYPTO_CRYPTRSA_HH
