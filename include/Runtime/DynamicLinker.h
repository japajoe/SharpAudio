/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#ifndef DYNAMICLINKER_HPP
#define DYNAMICLINKER_HPP

#if defined ( __GNUC__ ) && not defined ( __clang__ ) &&  __GNUC__ == 4 && ( __GNUC_MINOR__ < 8 || (  __GNUC_MINOR__ == 8 && __GNUC_PATCHLEVEL__ < 2 ) )
  #error GCC < 4.8.2 IS NOT SUPPORTED
#endif

#pragma once

#include <memory>
#include <iostream>
#include <functional>
#include <stdexcept>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

namespace DynamicLinker {

  class DynamicLinkerException : public std::runtime_error {
  public:
    explicit DynamicLinkerException ( const std::string& what_arg ) :
      std::runtime_error( what_arg ) {}
    explicit DynamicLinkerException ( const char* what_arg ) :
      std::runtime_error( what_arg ) {}
  };

  class OpenException : public DynamicLinkerException {
  public:
    explicit OpenException(std::string s) : DynamicLinkerException("ERROR: Cannot open dynamic library: " + s) {}
  };

  class SymbolException : public DynamicLinkerException {
  public:
    explicit SymbolException(std::string s) : DynamicLinkerException("ERROR: Cannot find symbol: " + s) {}
  };

  class ClosedException : public DynamicLinkerException {
  public:
    explicit ClosedException() : DynamicLinkerException("ERROR: Library was not opened!") {}
  };

  class SymbolInitException : public DynamicLinkerException {
  public:
    explicit SymbolInitException() : DynamicLinkerException("ERROR: Symbol was not initialized!") {}
  };

  class DynamicLinker : public std::enable_shared_from_this<DynamicLinker> {
  private:

    bool closeLib();
    void * openLib();

    class _void {
    private:
      void * myself = nullptr;
    public:
      explicit _void( void * );
      ~_void();
      void * ptr() const;
      void null();
    };


    template<typename T> class dlSymbol {};

    template<typename R, typename ...A> class dlSymbol< R(A...) >  {
    private:
      std::function< R(A...) > sym;
      std::shared_ptr<DynamicLinker> parent = nullptr;
      std::string name = "";
      void * getSymbol() {
        #ifdef _WIN32
          return GetProcAddress( (HINSTANCE) parent->lib->ptr() , name.c_str() );
        #else
          return dlsym( parent->lib->ptr(), name.c_str() );
        #endif
      }

    public:
      dlSymbol( std::shared_ptr<DynamicLinker> p, std::string n )
        : parent(p), name(n) {
          sym = std::function< R(A...) >(nullptr);
        }
      R operator()(A... arg) {
        if( sym != nullptr ) {
          return sym(arg...);
        }
        throw SymbolInitException();
      }
      std::function< R(A...) > raw() {
        return sym;
      }
      void init() {

        if( parent->lib == nullptr ) {
          throw ClosedException();
        }

        sym = std::function< R(A...) >(reinterpret_cast<  R(*)(A...)  >( getSymbol() ));

        if( sym == nullptr ) {
          char* err = nullptr;  //dlerror();
          std::string s = (err == nullptr) ? "FATAL ERROR: No error!" : std::string(err);
          throw SymbolException(s);
        }
      }
    };


    std::string libPath = "";
    std::unique_ptr<_void> lib = std::unique_ptr<_void>(nullptr);
    DynamicLinker();
    explicit DynamicLinker( std::string );
  public:
    static std::shared_ptr<DynamicLinker> make_new( std::string );
    ~DynamicLinker();
    bool open();
    template<typename T> dlSymbol<T> getFunction( std::string name ) {
      return dlSymbol<T>( shared_from_this(), name);
    }
  };

}; // namespace dynamicLinker

#endif