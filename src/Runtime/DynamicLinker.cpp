/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#include "DynamicLinker.h"

namespace DynamicLinker {

  bool DynamicLinker::closeLib() {
#ifdef _WIN32
  return FreeLibrary( (HINSTANCE) lib->ptr() );
#else
  return ( dlclose( lib->ptr() ) == 0 );
#endif
  }

  void * DynamicLinker::openLib() {
#ifdef _WIN32
  return LoadLibrary( libPath.c_str() );
#else
  return dlopen( libPath.c_str(), RTLD_NOW | RTLD_LOCAL );
#endif
  }

  DynamicLinker::DynamicLinker( std::string path ) : libPath(path) {
  }

  std::shared_ptr<DynamicLinker> DynamicLinker::make_new( std::string path ) {
    auto x = new DynamicLinker(path);
    return std::shared_ptr<DynamicLinker>(x);
  }

  bool DynamicLinker::open() {
    // C++14:
    // lib = std::make_unique<_void>( dlopen( libPath.c_str(), RTLD_NOW | RTLD_LOCAL ) );
    auto v = new _void( openLib() );
    lib = std::unique_ptr<_void>( v );

    if ( lib->ptr() == nullptr ) {
      lib = nullptr;
      char* err = nullptr;  //dlerror();
      std::string s = (err == nullptr) ? "FATAL ERROR: no error!" : std::string(err);
      throw OpenException(s);
    }

    return true;
  }

  DynamicLinker::~DynamicLinker() {
    if( lib != nullptr ) {
      if( ! closeLib() ) {
        lib.reset();
        return;
      }
      lib->null();
      lib = nullptr;
    }
  }

  DynamicLinker::_void::_void( void * ptr ) : myself(ptr) {}

  DynamicLinker::_void::~_void() {
    if( myself != nullptr ) {
      std::free(myself);
      myself = nullptr;
    }
  }

  void * DynamicLinker::_void::ptr() const {
    return myself;
  }

  void DynamicLinker::_void::null() {
    myself = nullptr;
  }

}; // namespace DynamicLinker