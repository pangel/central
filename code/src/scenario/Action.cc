
#include "Action.h"

Action::Action(std::string n) {
  name = n;
  this->playerID = this->simulation->getPlayer()->getID () ;
};

bool Action::isActionPossible(){
  std::cerr << "isActionPossible called but not implemented for the Action type :" << name << "\n";
  return 0;
};

void Action::doAction(){
  std::cerr << "doAction called but not implemented for the Action type :" << name << "\n";
  return;
};

