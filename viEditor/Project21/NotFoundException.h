#pragma once
//  Created by Frank M. Carrano and Timothy M. Henry.
//  Copyright (c) 2017 Pearson Education, Hoboken, New Jersey.

/** Listing 7-5.
@file NotFoundException.h */

#ifndef NOTFOUND_EXCEP_
#define NOTFOUND_EXCEP_

#include <stdexcept>
#include <string>

class NotFoundException : public std::logic_error
{
public:
	NotFoundException(const std::string& message = "");
}; // end NotFoundException 
#endif