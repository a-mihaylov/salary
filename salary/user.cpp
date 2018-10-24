#include "user.h"


User::User(int id, int authority) {
  this->id = id;
  this->authority = authority;
}


User::~User() {
}

bool User::canCreateUser() const {
  return (this->authority & this->kCreateuser) == this->kCreateuser;
}

bool User::canReadDatabase() const {
  return (this->authority & this->kReadDatabase) == this->kReadDatabase;
}