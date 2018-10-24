#pragma once
//Класс сотрудника
class User
{
public:
  explicit User(int id, int authority);
  ~User();

  const static int kReadDatabase = 1 << 0;
  const static int kCreateuser = 1 << 1;

  bool canCreateUser() const;

  bool canReadDatabase() const;

private:
  int id;
  int authority;
};

