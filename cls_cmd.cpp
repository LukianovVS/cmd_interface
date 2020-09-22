#include "cls_cmd.h"
#include <string.h>
#include <windows.h>
#include <algorithm>

//DBG
#include <iostream>



extern VOID ErrorExit (const char * const err_msg);

// ѕолучить кол-во цифр дес€тичного числа
//int nDig(int N)
//{
//  return (N == 0) ? 1 : log10(abs(N)) + 1;
//}

CMD_HANDLER cmd_handler;


//! функци€ дл€ упор€довачивани€ списка команд. чтобы не захломл€ть класс, вынес еЄ отдельно
bool func_cmptoini(CMD_LIST a, CMD_LIST b)
{
//  char aa[122];
//  char bb[122];
//  strcpy(aa, a.name.c_str());
//  strcpy(bb, b.name.c_str());
//  int i  = strcmp(a.name.c_str(), b.name.c_str());
//  int ii = strcmp(b.name.c_str(), a.name.c_str());
  if ( strcmp(a.name.c_str(), b.name.c_str()) < 0 )
    return true;
  else
    return false;
}


CMD_HANDLER::CMD_HANDLER()
{
  Ncmd  = 0;
  i_cmd = 0;
}

CMD_HANDLER::~CMD_HANDLER()
{
  //dtor
}

/*! поиск строки по маске
 *
 */
bool CMD_HANDLER::findstr(int * const ind, const std::string * const *cmd_mask, const std::string * const  str_arr, int * const i, const int N)
{

}

/*! ѕоиск команды по маске
 * ind - возврращаетс€ номер найденной команды, который удовлетвор€ет заданной маске
 * cmd_mask - строка, котора€ содержит маску дл€ поиска команды ( например, если маска = "get", то строка "getXYZ" - подходит под указанную маску)
 * continueFind - продолжить поиск (true), или начать сначала (false)
 * return - true если найдена команда, котора€ удовлетвор€ет маске
 */
bool CMD_HANDLER::findCmd(int * const ind, const std::string * const cmd_mask, bool compliteCoincidence, bool continueFind)
{
  bool retval = false;
  size_t lenmask = cmd_mask->length();

  if (continueFind == false)
    this->i_cmd = 0;


  for ( int i = 0 ; i < this->Ncmd; i++, this->i_cmd++)
  {
    if (this->i_cmd >= this->Ncmd)
      this->i_cmd -= this->Ncmd;
    int ind_cmp = cmd_mask->compare(0, lenmask, this->cmd[this->i_cmd].name.c_str(), lenmask);

    if (  ind_cmp == 0 )
    {
      if ( (compliteCoincidence && lenmask == this->cmd[this->i_cmd].name.length() ) || !compliteCoincidence)
      {
        retval = true;
        *ind = this->i_cmd;
        this->i_cmd++; // на случай, если в следующем заходе надо будет проболжить поиск, то мы продолжим со следующего эл-та
        return true;
      }
    }
  }
  this->i_cmd = 0; // на случай, если в следующем заходе надо будет проболжить поиск, то мы начнем его с нул€
  return false;
}


/*! ѕоиск команды по маске среди введеных ранее
 * ind - возврращаетс€ номер найденной команды, который удовлетвор€ет заданной маске
 * cmd_mask - строка, котора€ содержит маску дл€ поиска команды ( например, если маска = "get", то строка "getXYZ" - подходит под указанную маску)
 * continueFind - продолжить поиск (true), или начать сначала (false)
 * return - true если найдена команда, котора€ удовлетвор€ет маске
 */
bool CMD_HANDLER::findToLegend(int * const ind, const std::string * const *cmd_mask, bool compliteCoincidence, bool continueFind)
{

}

//! «апуск команды c индексом (номером) ind. \—оотвествующий индекс можно получить в findCmd
void CMD_HANDLER::callCmd(const int ind, std::string *args)
{
  if ( ind > 0 && ind < this->Ncmd)
  {
    std::string os(this->cmd[ind].call.c_str());
    os.append(" ");
    os.append(args->c_str());
    system(os.c_str());
  }
  this->i_cmd = 0;
}

/*! »нициализаци€ полей "name", "cmd", "help"
 */
bool CMD_HANDLER::init(const std::string * const f_ini)
{
  cmd_path = ".\\";

  const int TMP_BUFF_SIZE = 200;
  char tmp_buff[TMP_BUFF_SIZE + 1];             // все секции будут записаны в этот буфер

  // ќпределе€м кол-во команд = кол-во секций - 1 (перва€ секци€ - это не команда, а обща€ конфигураци€)
  int N = GetPrivateProfileSectionNames(tmp_buff, TMP_BUFF_SIZE, f_ini->c_str()) - 1;
  for (int i = 0; i < N; i++)
  {
    if (tmp_buff[i] == '\0')        // все секции разделены в буфере символом конца строки. ≈сть новый символ конца строки -> счетчик кол-ва секций++
      this->Ncmd++;
  }

  int k = 0;
  // читаем подр€д команды из ini файла
  for (int i = 0; i < _CFG_NUM_CMD_MAX_; i++)
  {
    char cmd_n[5 + 3 + int(_CFG_NUM_CMD_MAX_/1000) ];
    sprintf(cmd_n, "cmd %d", i + 1);
    GetPrivateProfileString(cmd_n, "name", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
    if (strlen(tmp_buff) > 0)
    {
      this->cmd[k].name = tmp_buff;
      GetPrivateProfileString(cmd_n, "comand", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
      this->cmd[k].call = tmp_buff;
      GetPrivateProfileString(cmd_n, "help", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
      this->cmd[k].help = tmp_buff;
      // DBG
      std::cout << k << ": " << this->cmd[k].name.c_str() << std::endl;
      // end DBG
      k++;
    }
  }

  if (this->Ncmd != k)
    ErrorExit("CMD_LIST Num cmd");

  std::sort(this->cmd, this->cmd + this->Ncmd, func_cmptoini);

  // DBG
  for (int i = 0; i < this->Ncmd; i++)
    std::cout << i << ": " << this->cmd[i].name.c_str() << std::endl;

  std::string tmp("echo");
  std::cout << tmp.c_str() << std::endl;
  for (int i = 0; i < 5; i++)
  {
    int a = -1;
    this->findCmd(&a, &tmp, false, true);
    std::cout << i << ": " << a <<std::endl;
  }

}
