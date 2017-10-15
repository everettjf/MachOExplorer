//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef DEFINE_H
#define DEFINE_H

#include <cstdlib>

struct cfstring_t
{
  uint32_t ptr;
  uint32_t data;
  uint32_t cstr;
  uint32_t size;
};

struct cfstring64_t
{
  uint64_t ptr;
  uint64_t data;
  uint64_t cstr;
  uint64_t size;
};

#endif // DEFINE_H
