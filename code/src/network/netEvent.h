#ifndef NETEVENT_H
#define NETEVENT_H

#include "abstractMessage.h"

/**
 * @brief The NetEvent class
 * Class used to communicate between the server and the
 * client. Provides a mean to notify the user about connexion
 * event. Some of these events can be generated by the implementation
 * of the server/client interface. Some other are only generated
 * expicitely by the client.
 * NetEvent will be handled in a particular way both by the client and the server.
 * The particular behaviours are the following :
 *  - SERV_TRY(resp CLI_TRY) will not be passed to the user when they are received. A
 *  SERV_RESP(resp CLI_RESP) message will be directly answered, the message is then discarded
 *
 *  - PLAYER_JOIN : the content of the message will be used to identify the Player in the future.
 */

class NetEvent : public AbstractMessage{
public:

  /**
   * @brief EventType : type used to descrive events.
   */
  typedef char EventType ;

  /**
   * @brief SERV_LOST : message generated when the server does not answer for a long time
   * data is NULL
   */
  static const EventType SERV_LOST = 1 ;
  /**
   * @brief SERV_TRY : send a message to the server to see if it responds
   * The server answers by a SERV_RESP message
   * data is NULL
   */
  static const EventType SERV_TRY = 2 ;
  /**
   * @brief SERV_RESP : server answer to SERV_TRY. Must not be sent manually
   * data is NULL
   */
  static const EventType SERV_RESP = 3 ;
  /**
   * @brief CLI_LOST : generated when the client does not respond for a long time
   * data is NULL
   */
  static const EventType CLI_LOST = 4 ;
  /**
   * @brief CLI_TRY : tries to contact the client. If it receives the message,
   * the client answers with CLI_RESP
   * data is NULL
   */
  static const EventType CLI_TRY = 5 ;
  /**
   * @brief CLI_RESP : client's answer to CLI_TRY. Must not be sent manually
   * data is NULL
   */
  static const EventType CLI_RESP = 6 ;
  /**
   * @brief MSG_LOST : message generated when a message could not be delivered.
   * data is of type (int *), the integer will be number associated with the message lost
   */
  static const EventType MSG_LOST = 7 ;
  /**
   * @brief PLAYER_JOIN : send this message to notify the server or other players
   * that a player joined the game.
   * data is of type (int *), the integer is the id of the player that joined the game.
   */
  static const EventType PLAYER_JOIN = 8 ;
  /**
   * @brief PLAYER_QUIT : send this message to notify the server or other players
   * that a player left the game
   * data is of type (int *), the integer is the id of the player that left the game.
   */
  static const EventType PLAYER_QUIT = 9 ;

  /**
   * @brief SEND_ERROR : generated when an error occured while
   * sending a message
   * data is of type (int *), the integer is the number associated with the message
   */
  static const EventType SEND_ERR = 10 ;

  /**
   * @brief RECEIVE_ERR : generated when an error occured when
   * receiving a message.
   * data is null.
   */
  static const EventType RECEIVE_ERR = 11 ;

  static const std::string MSG_TYPE ;


  /**
   * @brief NetEvent : creates a new NetEvent instance.
   * The type is set to 0 by default. A type of 0 is meant to
   * carry no meaning.
   */
  NetEvent();

  /**
   * @brief NetEvent : creates a new NetEvent with the given type
   */
  NetEvent(int type) ;

  /**
   * @brief getType
   * @return the type of this message
   */
  EventType inline getType(){ return type ;}

  /**
   * @brief setType
   * @param type : the type to set to this message
   */
  void inline setType(EventType type){this->type = type ;}



  /*
   * Methods inherited from the AbstractMessage interface
   */
  static std::string getMsgType() ;
  /**
   * @brief toString : unimplemented yet
   */
  virtual std::string & toString() ;
  /**
   * @brief fromString : unimplemented yet
   */
  static NetEvent * fromString(std::string& msg) ;

  virtual NetEvent* copy() ;


protected :
  /**
   * @brief type : the type of this message.
   * The possible types are described in static fields.
   */
  EventType type ;

  /**
   * @brief data : the data that is attached to this message.
   * Each message type should define what is the type of data
   * attached with the message.
   * It is left to the user to ensure that the correct data type is used for
   * a given message type
   */
  void * data ;

};

#endif // NETEVENT_H
