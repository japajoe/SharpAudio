/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
#ifndef FILESYSTEM_UTILIS_HPP
#define FILESYSTEM_UTILS_HPP

#include <experimental/filesystem>

void AddFilesFromDirectoryToTpaList( const std::string& directory, std::string& tpaList ) {

    for ( auto& dirent : std::experimental::filesystem::directory_iterator(directory) ) {
        std::string path = dirent.path();

        if ( ! path.compare(path.length() - 4, 4, ".dll") ) {
            tpaList.append(path + ":");
        }
    }

}

#endif