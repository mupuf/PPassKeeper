#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>


// Convertit n'importe quoi en un std::string
template <class T>
inline std::string toString(T a)
{
   std::ostringstream oss;
   oss << a;
   return oss.str();
}

#ifdef WIN32
// specialisation pour le type int
template <>
inline std::string toString(int a)
{
   char buffer[sizeof(int)*4];
   _itoa(a, buffer, 10);
   return std::string(buffer);
}
#endif

// Convertit un std::string en n'importe quoi
template <class T>
inline T stringTo(const std::string& a)
{
   T res;
   std::stringstream oss;
   oss << a;
   oss >> res;
   return res;
}

// specialisation pour le type int
template <>
inline int stringTo(const std::string& a)
{
   return atoi(a.c_str());
}

/// classe permettant de gerer les tokens au sein d'un std::string
class tokenizer 
{
private:
   inline static const std::string& getnullstring()
   {
      static std::string ret;
      return ret;
   };
public:
   /// constructeur : par defaut, le separateur est l'espace
   inline explicit tokenizer ( const std::string& s, char delim = ' ' ) : original(s), delimiteur(delim)
   {
      
      std::string token;
      std::string::size_type front = 0;
      std::string::size_type back = 0;

      for (;;)
      {
         if ( back == std::string::npos )
            break;
         front = s.find_first_not_of ( delim, front );
         if ( front == std::string::npos )
            break;
         back = s.find_first_of ( delim, front );
         token = s.substr ( front, back - front );
         tokens.push_back ( token );
         front = back + 1;
      }
      current = tokens.begin();
   };

   /// renvoie le std::string compris entre le a ieme et le b ieme token
   inline std::string gettok(int a, int b) const
   {
      bool was_tok = true;
      int compteur = 0;
      std::string::size_type debut = (unsigned int) -1;
      std::string::size_type fin = 0;
      if (a < 0)
         a += ( (int)tokens.size() + 1);
      if (b < 0)
         b += ( (int)tokens.size() + 1);
      register std::string::size_type taille = original.size();
      for (std::string::size_type i = 0; i < taille; i++)
      {
         if (original[i] != delimiteur && was_tok)
         {
            compteur++;
            was_tok = false;
            if (compteur == a)
               debut = i;
         }
         else if (original[i] == delimiteur && !was_tok)
         {
            if (compteur == b)
               fin = i;
            was_tok = true;
         }
      }
      if (debut != (unsigned int) -1)
      {
         if (fin == 0)
            fin = taille;
         if (fin > debut)
         {
            return original.substr(debut, fin-debut);
         }
      }
      return std::string();
   };

   /// renvoie le token numero i
   inline const std::string& gettok(int i) const
   {
      if (i == 0 || tokens.size() == 0)
         return getnullstring();
      if (i < 0)
         i+= tokens.size()+1;
      if (i <= 0 || (unsigned int)i > tokens.size())
         return getnullstring();
      return tokens[i-1];
   };

   inline const std::string& $(int i) const
   {
      return gettok(i);
   };

   /// renvoie le nombre de tokens
   inline std::string::size_type numtok() const
   {
      return tokens.size();
   };

   inline const std::string& next()
   { 
      return !done() ? *current++ : getnullstring();
   };

   inline bool done() const 
   {
      return current == tokens.end();
   };

   /// permet de savoir si un token donne est present dans la chaine
   inline bool istok(const std::string& tok) const
   {
      for (unsigned int i = 0; i < tokens.size(); i++)
      {
         if (tokens[i] == tok)
            return true;
      }
      return false;
   };

private:
   std::string original;
   std::vector<std::string> tokens;
   std::vector<std::string>::iterator current;
   char delimiteur;
};


inline int myTolower(int a)
{
   if ((a < 'A') || (a > 'Z'))
      return a;
   return (a + ('a' - 'A'));
}


// specialisation de ces operateurs pour la classe std::string
// maintenant par defaut : on ne tient pas compte de la casse

inline bool operator==(const std::string& a, const char* b)
{
   register std::string::size_type size_a = a.size();
   for (std::string::size_type i = 0; i < size_a; i++)
   {
      if (b[i] == 0 || (myTolower(a[i]) != myTolower(b[i])))
         return false;
   }
   return (b[size_a] == 0); // on verifie que b a la meme taille que a
}

inline bool operator!=(const std::string& a, const char* b)
{
   return !(a == b);
}

inline bool operator==( const char* a, const std::string& b)
{
   return (b == a);
}

inline bool operator!=( const char* a, const std::string& b)
{
   return !(a == b);
}

inline bool operator==(const std::string& a, const std::string& b)
{
   register std::string::size_type size_a = a.size();
   if (size_a != b.size())
      return false;
   for (std::string::size_type i = 0; i < size_a; i++)
   {
      if ((myTolower(a[i]) != myTolower(b[i])))
         return false;
   }
   return true;
}

inline bool operator!=( const std::string& a, const std::string& b)
{
   return !(a == b);
}


inline bool operator<(const std::string& a, const char* b)
{
   std::string::size_type size_a = a.size();
   for (std::string::size_type i = 0; i < size_a; i++)
   {
      int a_i = myTolower(a[i]);
      int b_i = myTolower(b[i]);

      if (b_i == 0)
         return false;
      if (a_i != b_i)
         return (a_i < b_i);
   }
   return (b[size_a] != 0);
}

inline bool operator>(const char* a, const std::string& b)
{
   return (b < a);
}


inline bool operator<(const char* a, const std::string& b)
{
   std::string::size_type size_b = b.size();
   for (std::string::size_type i = 0; i < size_b; i++)
   {
      int a_i = myTolower(a[i]);
      int b_i = myTolower(b[i]);

      if (a_i == 0)
         return true;
      if (a_i != b_i)
         return (a_i < b_i);
   }
   return false;
}

inline bool operator>(const std::string& a, const char* b)
{
   return (b < a);
}

inline bool operator<(const std::string& a, const std::string& b)
{
   std::string::size_type size_a = a.size();
   std::string::size_type size_b = b.size();

   register std::string::size_type minsize = (size_a < size_b ? size_a : size_b);
   for (std::string::size_type i = 0; i < minsize; i++)
   {
      int a_i = myTolower(a[i]);
      int b_i = myTolower(b[i]);
      if (a_i != b_i)
         return (a_i < b_i);
   }
   return (size_a < size_b);
}

inline bool operator>(const std::string& a, const std::string& b)
{
   return (b < a);
}
inline bool operator<=(const std::string& a, const std::string& b)
{
   return !(a > b);
}

inline bool operator<=(const char* a, const std::string& b)
{
   return !(a > b);
}

inline bool operator<=(const std::string& a, const char* b)
{
   return !(a > b);
}

inline bool operator>=(const std::string& a, const std::string& b)
{
   return !(a < b);
}

inline bool operator>=(const char* a, const std::string& b)
{
   return !(a < b);
}

inline bool operator>=(const std::string& a, const char* b)
{
   return !(a < b);
}

inline std::string operator << (const std::string& a, int num)
{
   if (num < 0)
      num+=a.size();
   if (num < 0 || (unsigned int)num >= a.size())
      return std::string();
   return a.substr( num, a.size()-num);
}

inline std::string operator >> (const std::string& a, int num)
{
   if (num < 0)
      num+=a.size();
   if (num < 0 || (unsigned int)num >= a.size())
      return std::string();
   return a.substr( 0, a.size() - num);
}

inline bool isnum(const std::string& s)
{
   std::string::size_type i = 0;
   std::string::size_type taille = s.size();
   if (taille > 0 && (s[0] == '+' || s[0] == '-'))
      i++;
   while (i < taille)
   {
      if (s[i] < '0' || s[i] > '9')
         return false;
      i++;
   }
   return true;
}

inline bool IsCharInString(const std::string& s, char c)
{
   register std::string::size_type taille = s.size();
   for (std::string::size_type i = 0; i < taille; i++)
   {
      if (s[i] == c)
         return true;
   }
   return false;
}

// classe de comparaison de chaînes de caractères :
class StringLessNoCase
{
public:
   inline bool operator()(const std::string &s1, const std::string &s2) const
   {
      return (s1 < s2);
   };
};

///// fin de la specialisation /////

#endif

