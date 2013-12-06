#include "trajectory.h"
#include "../generation/tile.h"
#include "tilewrapper.h"

#include <vector>
#include <list>
#include <cassert>
#include <queue>

Trajectory::Trajectory() {
  Position p1,p2;
  p1 = Position();
  p2 = Position();
  posList = std::list<Position> ();
  posList.push_front(p2);
  posList.push_front(p1);
  hasArrived = false;
  return;
}

Trajectory::Trajectory(Position start,Position target,Geography& map) {
  posList = std::list<Position> ();
  posList.push_front(target);
  posList.push_front(start);
  pathfinding(map);
  hasArrived = false;
  return;
}

Trajectory::Trajectory(Trajectory& t) {
  posList = std::list<Position>(t.getPosList());
  hasArrived = t.getHasArrived();
  return;
}


void Trajectory::explore(TileWrapper* y,TileWrapper* z,std::priority_queue<TileWrapper*,std::vector<TileWrapper*>,TileWrapperComparator>& open) {
  if (!y->isClosed()) {
    if (y->isOpen()) {
      if (z->getDistance()+1<y->getDistance()) {
        y->setParent(z);
        y->setDistance(z->getDistance()+1);
      }
    } else {
      open.push(y);
      y->setOpen(true);
      y->setParent(z);
      y->setDistance(z->getDistance()+1);
    }
  }
  return;
}

void Trajectory::pathfinding(Geography& map) {//stupide pour l'instant, A* apres
  assert(posList.size()>1);//le vecteur doit contenir au moins le départ et l'arrivée
  Position start,target;
  start = posList.front();
  target = posList.back();
  posList.clear();
  
  std::priority_queue<TileWrapper*, std::vector<TileWrapper*>, TileWrapperComparator> open;
  std::list<TileWrapper*> closed;

  Tile& tileStart = start.isInTile(map);
  Tile& tileTarget = target.isInTile(map);

  TileWrapper* s = new TileWrapper(&tileStart,tileTarget);
  s->setDistance(0);
  s->setOpen(true);
  open.push(s);

  TileWrapper* t = new TileWrapper(&tileTarget,tileTarget);

  TileWrapper* temp;
  bool found = false;
  
  while (!open.empty() && !found) {
    TileWrapper* z = open.top();
    open.pop();
    if (z->equals(*t)) {
      temp = z;
      found = true;
    } else {
      z->setClosed(true);
      closed.push_front(z);

      std::list<Tile*> neighbourhood;
      if (z->getTile().getGou()) {
        neighbourhood.push_front(map.getTile(z->getTile().getCoord().getAbs()-1,z->getTile().getCoord().getOrd()));
      }
      if (z->getTile().getGol()) {
        neighbourhood.push_front(map.getTile(z->getTile().getCoord().getAbs(),z->getTile().getCoord().getOrd()-1));
      }
      if (z->getTile().getGor()) {
        neighbourhood.push_front(map.getTile(z->getTile().getCoord().getAbs(),z->getTile().getCoord().getOrd()+1));
      }
      if (z->getTile().getGoh()) {
        neighbourhood.push_front(map.getTile(z->getTile().getCoord().getAbs()+1,z->getTile().getCoord().getOrd()));
      }
      
      while (!neighbourhood.empty()) {
        Tile tempTile = *(neighbourhood.front());
        neighbourhood.pop_front();
        TileWrapper* y = tempTile.getWrapper();
        if (y) {
          explore(y,z,open);
        } else {
          TileWrapper* y = new TileWrapper(&tempTile,tileTarget);
          explore(y,z,open);
        }
      }
    }
  }
  assert(found);//si la map est pas connexe voir avec Chatan
  
  posList.push_front(target);
  while (!temp->equals(*s)) {
    temp = temp->getParent();
    assert(temp);
    posList.push_front(Position(temp->getTile()));
  }
  posList.pop_front();
  posList.push_front(start);

  while(!open.empty()) {
    TileWrapper* tempWrapper = open.top();
    open.pop();
    tempWrapper->getTile().resetWrapper();
  }

  while(!closed.empty()) {
    TileWrapper* tempWrapper = closed.front();
    closed.pop_front();
    tempWrapper->getTile().resetWrapper();
  }
  return;
}
                
      
std::list<Position>& Trajectory::getPosList() {
  return posList;
}


Position& Trajectory::getPosition() {
  return posList.front();
}

void Trajectory::setPosition(Position& p) {
  posList.pop_front();
  posList.push_front(p);
  return;
}

bool Trajectory::getHasArrived() {
  return hasArrived;
}

void Trajectory::update(sf::Time dt,float speed,Geography& map) {
  assert(!posList.empty());//il doit y avoir au moins la position courante  
  if (!hasArrived) {//si on n'est pas arrivé : on avance en ligne droite
    assert(posList.size()>1);//il doit y avoir la position courante et au moins un objectif
    float dist = dt.asSeconds()*speed;
    Position position = posList.front();
    posList.pop_front();
    Position& target = posList.front();
    float dX = target.getX()-position.getX();
    float dY = target.getY()-position.getY();
    float norm = (float)sqrt(pow(dX,2)+pow(dY,2));
    dX = dX/norm;
    dY = dY/norm;
    dX = dX*dist;
    dY = dY*dist;
    position.add(dX,dY);

    float dist1 = position.distance(target);
    
    if (dist1 <= dist/2) {//on est assez proche de l'objectif
      posList.pop_front();
      if (posList.empty()) {//il ne reste plus que la position courante
        hasArrived = true;
      }
    }
    posList.push_front(position);
  }
  return;
}
