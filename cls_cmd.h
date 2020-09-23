#ifndef CLS_CMD_H
#define CLS_CMD_H

#include <string>


//! максимальное количество команд
#define _CFG_NUM_CMD_MAX_ 128
//! максимальная длина имени команды
#define _CFG_LEN_CMD_NAME_   64
//! количество послежних запоминаемых команд
#define _CFG_LEGEND_SIZE_ 20


// TODO (#4) надо продумать класс CMD_LOG
//class CMD_LOG
//{
//public:
//  void append(const std::string * const s)
//  bool get(int i);
//private:
//
//};


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


    /*! Поиск команды по маске
     * ind - возврращается номер найденной команды, который удовлетворяет заданной маске
     * cmd_mask - строка, которая содержит маску для поиска команды ( например, если маска = "get", то строка "getXYZ" - подходит под указанную маску)
     * continueFind - продолжить поиск (true), или начать сначала (false)
     * return - true если найдена команда, которая удовлетворяет маске
     */
    bool findCmd(int * const ind, const std::string * const cmd_mask, bool compliteCoincidence, bool continueFind);

    inline void reset_find_ind() {i_cmd = 0;}


    //! Запуск команды c индексом (номером) ind. \Соотвествующий индекс можно получить в findCmd
    void callCmd(const int ind, std::string *args);

    /*! Инициализация полей "name", "cmd", "help"
     */
    bool init(const std::string * const f_ini);

    bool getName(std::string * const str, int ind);

  private:


    // TODO (#7) можно перейти на динамический массив
    CMD_LIST cmd[_CFG_NUM_CMD_MAX_];
    // номер команды с которой будет продолжен поиск ( в случае, если будет запрос именно продолжить поиск)
    int i_cmd;

    // фактическое кол-во команд
    int Ncmd;

    std::string legend[_CFG_LEGEND_SIZE_];
};


extern CMD_HANDLER cmd_handler;

#endif // CLS_CMD_H
