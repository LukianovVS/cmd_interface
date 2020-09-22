#ifndef CLS_CMD_H
#define CLS_CMD_H

#include <string>


//! ������������ ���������� ������
#define _CFG_NUM_CMD_MAX_ 128
//! ������������ ����� ����� �������
#define _CFG_LEN_CMD_NAME_   64
//! ���������� ��������� ������������ ������
#define _CFG_LEGEND_SIZE_ 20


class CMD_LIST
{
public:
//  CMD_LIST();

  std::string name;
  std::string call;
  std::string help;
};



class CMD_HANDLER
{
  public:
    CMD_HANDLER();
    virtual ~CMD_HANDLER();


    /*! ����� ������� �� �����
     * ind - ������������� ����� ��������� �������, ������� ������������� �������� �����
     * cmd_mask - ������, ������� �������� ����� ��� ������ ������� ( ��������, ���� ����� = "get", �� ������ "getXYZ" - �������� ��� ��������� �����)
     * continueFind - ���������� ����� (true), ��� ������ ������� (false)
     * return - true ���� ������� �������, ������� ������������� �����
     */
    bool findCmd(int * const ind, const std::string * const cmd_mask, bool compliteCoincidence, bool continueFind);


    /*! ����� ������� �� ����� ����� �������� �����
     * ind - ������������� ����� ��������� �������, ������� ������������� �������� �����
     * cmd_mask - ������, ������� �������� ����� ��� ������ ������� ( ��������, ���� ����� = "get", �� ������ "getXYZ" - �������� ��� ��������� �����)
     * continueFind - ���������� ����� (true), ��� ������ ������� (false)
     * return - true ���� ������� �������, ������� ������������� �����
     */
    bool findToLegend(int * const ind, const std::string * const *cmd_mask, bool compliteCoincidence, bool continueFind);

    //! ������ ������� c �������� (�������) ind. \�������������� ������ ����� �������� � findCmd
    void callCmd(const int ind, std::string *args);

    /*! ������������� ����� "name", "cmd", "help"
     */
    bool init(const std::string * const f_ini);



  private:

    /*! ����� ������ �� �����
     *
     */
    bool findstr(int * const ind, const std::string * const *cmd_mask, const std::string * const  str_arr, int * const i, const int N);


    // TODO (#7) ����� ������� �� ������������ ������
    CMD_LIST cmd[_CFG_NUM_CMD_MAX_];
    // ����� ������� � ������� ����� ��������� ����� ( � ������, ���� ����� ������ ������ ���������� �����)
    int i_cmd;

    // ����������� ���-�� ������
    int Ncmd;

    // ����, ������������ �������� ������� �������
    std::string cmd_path;

    std::string legend[_CFG_LEGEND_SIZE_];
};


extern CMD_HANDLER cmd_handler;

#endif // CLS_CMD_H
