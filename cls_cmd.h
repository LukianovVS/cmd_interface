#ifndef CLS_CMD_H
#define CLS_CMD_H

#include <string>


//! максимальное количество команд
#define _CFG_NUM_CMD_MAX_ 128
//! максимальная длина имени команды
#define _CFG_LEN_CMD_NAME_   64
//! количество послежних запоминаемых команд
#define _CFG_LEGEND_SIZE_ 20


// TODO (#9) лог можно реализовать более красиво: массив объектов с указателями на следующий и предыдущий эл-ты.
// но пока не хочу заморячиваться...
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


// структура для хранения имени команды, пути к исполняемому файлу и подсказки.
class CMD_LIST
{
public:
  std::string name;
  std::string call;
  std::string help;
};


// основной класс для работы с командами
class CMD_HANDLER
{
  public:
    CMD_HANDLER();
    virtual ~CMD_HANDLER();


    /*! Поиск команды по маске
     * ind - возврращается номер найденной команды, который удовлетворяет заданной маске
     * cmd_mask - строка, которая содержит маску для поиска команды ( например, если маска = "get", то строка "getXYZ" - подходит под указанную маску)
     * compliteCoincidence - искать с полным совпадением
     * continueFind - продолжить поиск (true), или начать сначала (false)
     * return - true если найдена команда, которая удовлетворяет маске
     */
    bool findCmd(int * const ind, const std::string * const cmd_mask, bool compliteCoincidence, bool continueFind);

    //! обнулить позицию поиска
    inline void reset_find_ind() {i_cmd = 0;}


    //! Запуск команды c индексом (номером) ind. \Соотвествующий индекс можно получить в findCmd
    void callCmd(const int ind, std::string *args);

    /*! Инициализация полей "name", "cmd", "help"
     */
    bool init(const std::string * const f_ini);

    //! получить имя команды по индексу
    bool getName(std::string * const str, int ind);

    //! печать списка команд + подсказки
    void print_cmd_list();

  private:


    // TODO (#7) можно перейти на динамический массив
    CMD_LIST cmd[_CFG_NUM_CMD_MAX_];
    // номер команды с которой будет продолжен поиск ( в случае, если будет запрос именно продолжить поиск)
    int i_cmd;

    // фактическое кол-во команд
    int Ncmd;
};


extern CMD_HANDLER cmd_handler;

extern CMD_LOG cmd_log;

#endif // CLS_CMD_H
