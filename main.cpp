
#include <iostream>
#include <Windows.h>




// ��������� ��������� �������
COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);
// ����� � �������
VOID ErrorExit(LPSTR);



HANDLE hStdin;
HANDLE hStdout;

int main()
{
  bool flg_loop = true;

  INPUT_RECORD irInBuf;
  DWORD cNumRead;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if (hStdin == INVALID_HANDLE_VALUE)
        ErrorExit("GetStdHandle");



  std::cout << "Hello! I am cmd" << std::endl;
  std::cout << "  Type Esc to exit" << std::endl;
  do
  {

    if (! ReadConsoleInput(
                hStdin,      // input buffer handle
                &irInBuf,     // buffer to read into
                1,           // size of read buffer
                &cNumRead) ) // number of records read
      ErrorExit("ReadConsoleInput");

    // ������. ���� ��� ������ - ����������� � ������
    if(!cNumRead)
      continue;
    // ���� ������� �� �� ����������  - ������� �������
    if(irInBuf.EventType != KEY_EVENT)
      continue;
    //�� ������������ ���� ������� ������
    if(irInBuf.Event.KeyEvent.bKeyDown)
      continue;
    // ��������� �������
    switch (irInBuf.Event.KeyEvent.wVirtualKeyCode)
    {
      case VK_UP:
      {
        COORD position = GetConsoleCursorPosition(hStdout);
        position.Y -= 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_DOWN:
      {
        COORD position   = GetConsoleCursorPosition(hStdout);
        position.Y += 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_LEFT:
      {
        COORD position   = GetConsoleCursorPosition(hStdout);
        position.X -= 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_RIGHT:
      {
        COORD position   = GetConsoleCursorPosition(hStdout);
        position.X += 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_ESCAPE:
      {
        flg_loop = false; // ����� �� �����
        break;
      }
      default:
      {
        // ��������� ��� ��� ��������� ������ (� �� ��������� �������) + ����������� ��������
        if (irInBuf.Event.KeyEvent.uChar.AsciiChar >= 32 &&
            irInBuf.Event.KeyEvent.uChar.AsciiChar <= 127 )
        {
          std::cout << irInBuf.Event.KeyEvent.uChar.AsciiChar;
        }

        break;
      }
    }


  } while(flg_loop);

  return 0;
}



COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
    {
        return cbsi.dwCursorPosition;
    }
    else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };
        return invalid;
    }
}


VOID ErrorExit (LPSTR lpszMessage)
{
    fprintf(stderr, "%s\n", lpszMessage);

    ExitProcess(0);
}


