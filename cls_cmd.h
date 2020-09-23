#ifndef CLS_CMD_H
#define CLS_CMD_H

#include <string>


//! ������������ ���������� ������
#define _CFG_NUM_CMD_MAX_ 128
//! ������������ ����� ����� �������
#define _CFG_LEN_CMD_NAME_   64
//! ���������� ��������� ������������ ������
#define _CFG_LEGEND_SIZE_ 20


// TODO (#9) ��� ����� ����������� ����� �������: ������ �������� � ����������� �� ��������� � ���������� ��-��.
// �� ���� �� ���� ��������������...
class CMD_LOG
{
public:
  CMD_LOG();
  void append(const std::string * const s);
  bool get_previous(std::string * const str);
  bool get_next(std::string * const str);

private:
  std::string log[_CFG_LEGEND_SIZE_];
  unsigned long counter;
  int i_next;
  int i_offset;
  int i_log;
};


// ��������� ��� �������� ����� �������, ���� � ������������ ����� � ���������.
class CMD_LIST
{
public:
  std::string name;
  std::string call;
  std::string help;
};


// �������� ����� ��� ������ � ���������
class CMD_HANDLER
{
  public:
    CMD_HANDLER();
    virtual ~CMD_HANDLER();


    /*! ����� ������� �� �����
     * ind - ������������� ����� ��������� �������, ������� ������������� �������� �����
     * cmd_mask - ������, ������� �������� ����� ��� ������ ������� ( ��������, ���� ����� = "get", �� ������ "getXYZ" - �������� ��� ��������� �����)
     * compliteCoincidence - ������ � ������ �����������
     * continueFind - ���������� ����� (true), ��� ������ ������� (false)
     * return - true ���� ������� �������, ������� ������������� �����
     */
    bool findCmd(int * const ind, const std::string * const cmd_mask, bool compliteCoincidence, bool continueFind);

    //! �������� ������� ������
    inline void reset_find_ind() {i_cmd = 0;}


    //! ������ ������� c �������� (�������) ind. \�������������� ������ ����� �������� � findCmd
    void callCmd(const int ind, std::string *args);

    /*! ������������� ����� "name", "cmd", "help"
     */
    bool init(const std::string * const f_ini);

    //! �������� ��� ������� �� �������
    bool getName(std::string * const str, int ind);

    //! ������ ������ ������ + ���������
    void print_cmd_list();

  private:


    // TODO (#7) ����� ������� �� ������������ ������
    CMD_LIST cmd[_CFG_NUM_CMD_MAX_];
    // ����� ������� � ������� ����� ��������� ����� ( � ������, ���� ����� ������ ������ ���������� �����)
    int i_cmd;

    // ����������� ���-�� ������
    int Ncmd;
};


extern CMD_HANDLER cmd_handler;

extern CMD_LOG cmd_log;

#endif // CLS_CMD_H
