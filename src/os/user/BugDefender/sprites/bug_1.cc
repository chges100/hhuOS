/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[30 * 20 * 4 + 1];
} bug_1 = {
  30, 20, 4,
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\331\335C\331\331\335"
  "\065\000\000\000\000\000\000\000\000\331\331\331\066\330\334\334A\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\333\333\333\016\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\332\332\231\377\377\377\002\000\000\000\000\000\000\000"
  "\000\377\377\377\002\330\332\334\230\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\333\333\333\016\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\333\333\215\325\337\337\030\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\332\332\332>\333\333\333V\330\333\333w\330\333"
  "\333w\330\330\330V\331\331\335<\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\334"
  "\334\334\026\330\332\332\213\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\330\330\330!\331\333\333\177\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\332\332\334{\331\332\333\377\331\332\333\377"
  "\331\332\333\377\331\332\333\377\331\331\333y\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\331\331\333\200\333\333\333#\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\331\333\225"
  "\330\330\330\015\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\333\333\322\331\332\333"
  "\355\331\332\333\324\331\332\333\324\331\332\333\355\331\333\333\322\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\321\321\321\013\331\332\332\222\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\332\332\332\060\330\332\332v\000\000\000\000\000\000\000\000\000\000\000\000\330\330\330"
  "\032\331\331\333\177\331\332\332\265\331\333\333\322\331\333\333\322\331\332"
  "\332\265\331\331\333\177\330\330\330\032\000\000\000\000\000\000\000\000\000\000\000\000\330\332\332"
  "v\332\332\332\060\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\327\333\333@\331\331\334"
  "e\334\334\334;\331\333\333\177\331\332\333\366\331\332\333\377\331\332\333"
  "\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331"
  "\332\333\366\331\331\333\200\334\334\334;\331\331\334e\327\333\333@\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\001\331\333\333\205"
  "\332\332\333\314\332\332\333\314\332\332\333\314\332\332\333\314\332\332"
  "\333\314\332\332\333\314\332\332\333\314\332\332\333\314\332\332\333\314"
  "\332\332\333\314\331\333\333\205\377\377\377\001\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\331\333k\331\332\333\377\331\332\333\377"
  "\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332"
  "\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377"
  "\331\332\333\377\330\332\332h\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\334\334\334\026\337\337"
  "\337\020\000\000\000\000\000\000\000\000\331\331\333\340\331\332\333\377\331\332\333\377\331"
  "\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333"
  "\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331"
  "\332\333\377\331\332\333\335\000\000\000\000\000\000\000\000\337\337\337\020\334\334\334\026"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\337\337\337\010\330"
  "\332\333\236\332\332\332S\330\333\333c\333\333\333\070\331\332\333\377\331"
  "\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333"
  "\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331"
  "\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\326\326\326"
  "\070\331\331\333d\332\332\332S\330\332\333\236\337\337\337\010\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\331\333\333~\333\333\333*\000\000\000\000\000"
  "\000\000\000\326\326\336\037\331\332\333\377\331\332\333\377\331\332\333\377\331"
  "\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333"
  "\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331"
  "\332\333\377\331\332\333\377\335\335\335\036\000\000\000\000\000\000\000\000\330\330\330'"
  "\332\332\334{\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\330\330\330\015\330\333\333"
  "c\330\333\333i\000\000\000\000\000\000\000\000\000\000\000\000\277\277\277\004\331\332\333\355\331"
  "\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333"
  "\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331"
  "\332\333\377\331\332\333\377\331\332\333\377\331\332\333\354\377\377\377"
  "\003\000\000\000\000\000\000\000\000\000\000\000\000\330\333\333i\330\333\333c\330\330\330\015\000\000\000"
  "\000\330\330\330'\330\332\332h\327\327\327\023\000\000\000\000\000\000\000\000\377\377\377\001"
  "\332\332\334t\330\333\333c\330\332\332\213\331\332\333\377\331\332\333\377"
  "\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332"
  "\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377"
  "\331\332\333\377\332\332\334\211\330\333\333c\332\332\334t\377\377\377\001"
  "\000\000\000\000\000\000\000\000\327\327\327\023\330\332\332h\330\330\330'\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\331\331\331=\330\333\333b\000\000\000\000\325\325\325\014"
  "\331\333\333\322\331\332\333\377\331\332\333\377\331\332\333\377\331\332"
  "\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377"
  "\331\332\333\377\331\332\333\377\331\332\332\321\321\321\321\013\000\000\000\000\330"
  "\333\333c\331\331\335<\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\332\332\333\217\325\325\325\022\000\000\000\000\000\000"
  "\000\000\334\334\334\026\331\332\332\274\331\332\333\377\331\332\333\377\331\332"
  "\333\377\331\332\333\377\331\332\333\377\331\332\333\377\331\332\333\377"
  "\331\332\333\377\331\332\333\272\333\333\333\025\000\000\000\000\000\000\000\000\337\337\337"
  "\020\331\333\333\215\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\327\333\333G\332\332\332|\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\377\377\377\001\330\333\333O\331\332\333\263\331\332\333\362\331\332\333"
  "\377\331\332\333\377\331\332\333\361\330\332\333\262\333\333\333M\377\377"
  "\377\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\332\332\332|\327\333\333G\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\332\332\332Y\327\333\333"
  "@\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\333\333\333\016\333\333\333\016\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\327\333\333@\332\332\332Y\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000",
};

