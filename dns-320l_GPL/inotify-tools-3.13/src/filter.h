#ifndef __FILTER_H__
#define __FILTER_H__

typedef struct
{
   char  ft_name[128];
}ft_define;

const ft_define ft_table[]=
{
   {
      .ft_name = {".AppleDouble"},
   },
   {
      .ft_name = {".AppleDB"},
   },
   {
      .ft_name = {".DS_Store"},
   },
   {
      .ft_name = {".bin"},
   },
   {
      .ft_name = {".AppleDesktop"},
   },
   {
      .ft_name = {"Network Trash Folder"},
   },
   {
      .ft_name = {".!@#$recycle"},
   },
   {
      .ft_name = {".systemfile"},
   },
   {
      .ft_name = {"lost+found"},
   },
   {
      .ft_name = {"Nas_Prog"},
   },
   {
      .ft_name = {"P2P"},
   },
   {
      .ft_name = {"aMule"},
   },
};

#endif
