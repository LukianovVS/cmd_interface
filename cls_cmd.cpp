#include "cls_cmd.h"
#include <string.h>
#include <windows.h>
#include <algorithm>

//DBG
#include <iostream>



extern VOID ErrorExit (const char * const err_msg);

// �������� ���-�� ���� ����������� �����
//int nDig(int N)
//{
//  return (N == 0) ? 1 : log10(abs(N)) + 1;
//}

CMD_HANDLER cmd_handler;


//! ������� ��� ���������������� ������ ������. ����� �� ���������� �����, ����� � ��������
bool func_cmptoini(CMD_LIST a, CMD_LIST b)
{
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


/*! ����� ������� �� �����
 * ind - ������������� ����� ��������� �������, ������� ������������� �������� �����
 * cmd_mask - ������, ������� �������� ����� ��� ������ ������� ( ��������, ���� ����� = "get", �� ������ "getXYZ" - �������� ��� ��������� �����)
 * continueFind - ���������� ����� (true), ��� ������ ������� (false)
 * return - true ���� ������� �������, ������� ������������� �����
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
        this->i_cmd++; // �� ������, ���� � ��������� ������ ���� ����� ���������� �����, �� �� ��������� �� ���������� ��-��
        return true;
      }
    }
  }
  this->i_cmd = 0; // �� ������, ���� � ��������� ������ ���� ����� ���������� �����, �� �� ������ ��� � ����
  return false;
}



//! ������ ������� c �������� (�������) ind. \�������������� ������ ����� �������� � findCmd
void CMD_HANDLER::callCmd(const int ind, std::string *args)
{
  if ( ind >= 0 && ind < this->Ncmd)
  {
    std::string os(this->cmd[ind].call.c_str());
    os.append(" ");
    os.append(args->c_str());
    system(os.c_str());
  }
  this->i_cmd = 0;
}

bool CMD_HANDLER::getName(std::string * const str, int ind)
{
  if (ind >= 0 && ind < this->Ncmd)
  {
    *str = this->cmd[ind].name;
    return true;
  }
  return false;
}

/*! ������������� ����� "name", "cmd", "help"
 */
bool CMD_HANDLER::init(const std::string * const f_ini)
{
  //TODO (#7) ���� ������ �� ini �����
  bool flag_relative_path = true;
  std::string cmd_path = "";
  if (flag_relative_path)
  {
    int i = f_ini->find_last_of('/\\');
    cmd_path.append(".\\");
    cmd_path.append(*f_ini, 0, i + 1);
  }

  const int TMP_BUFF_SIZE = 200;
  char tmp_buff[TMP_BUFF_SIZE + 1];             // ��� ������ ����� �������� � ���� �����

  // ���������� ���-�� ������ = ���-�� ������ - 1 (������ ������ - ��� �� �������, � ����� ������������)
  int N = GetPrivateProfileSectionNames(tmp_buff, TMP_BUFF_SIZE, f_ini->c_str()) - 1;
  for (int i = 0; i < N; i++)
  {
    if (tmp_buff[i] == '\0')        // ��� ������ ��������� � ������ �������� ����� ������. ���� ����� ������ ����� ������ -> ������� ���-�� ������++
      this->Ncmd++;
  }

  int k = 0;
  // ������ ������ ������� �� ini �����
  for (int i = 0; i < _CFG_NUM_CMD_MAX_; i++)
  {
    char cmd_n[5 + 3 + int(_CFG_NUM_CMD_MAX_/1000) ];
    sprintf(cmd_n, "cmd %d", i + 1);
    GetPrivateProfileString(cmd_n, "name", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
    if (strlen(tmp_buff) > 0)
    {
      this->cmd[k].name = tmp_buff;
      GetPrivateProfileString(cmd_n, "comand", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
      this->cmd[k].call = cmd_path;
      this->cmd[k].call.append(tmp_buff);
      GetPrivateProfileString(cmd_n, "help", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
      this->cmd[k].help = tmp_buff;
      k++;
    }
  }

  if (this->Ncmd != k)
    ErrorExit("CMD_LIST Num cmd");

  std::sort(this->cmd, this->cmd + this->Ncmd, func_cmptoini);

  std::cout << "cmd list:\n";
  for (int i = 0; i < Ncmd; i++)
    std::cout << cmd[i].name.c_str() << " - " << cmd[i].help.c_str() << std::endl;
  std::cout << "---------------------------" << std::endl;

}
