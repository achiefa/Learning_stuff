#include <iostream>

#define MAX_CHARGE 10

//------------ INPUT ------------
enum Inputs
{
  RELEASE_DOWN,
  PRESS_DOWN
};

struct Input
{ 
  public:
    Input(char& input)
    : input_(input)
    {}

    char getInput() { return input_; }

    bool operator==(const char& lhs) {
      return input_ == lhs;
    }

  private:
    char input_;
};


//------------ CHARACTER ------------
// We delegate actions to states
class Character
{
  public:
    virtual void handleInput(Input input)
    {
      CharacterState* state = state_->handleInput(*this, input);
      CharacterState* equipment = equipment_->handleInput(*this, input);
      if(state != NULL)
      {
        delete state_;
        state_ = state;

        state_->enter(*this);
      }
    }

    virtual void update()
    {
      state_->update(*this);
    }

    void setGraphics() {
      std::cout << "Image set" << std::endl;
    }

    void setState(CharacterState* state) { 
      state_ = state;
    }

    virtual void superAttack() {
      std::cout << "Standard super attack" << std::endl;
    }

  private:
    CharacterState* state_;
    CharacterState* equipment_;
  
  friend class CharacterState;
};

//------------ CHARACTER STATES static ------------
class CharacterState // State interface
{
  public:
    virtual ~CharacterState() {}
    virtual CharacterState* handleInput(Character& character, Input inpput);
    virtual void update(Character& character);
    virtual void enter(Character&);
};


class StandingState : public CharacterState
{
  public:
    StandingState() {}

    virtual CharacterState* handleinput(Character& character, Input input) {
      if (input == PRESS_DOWN)
      {
        std::cout << "I was standing and then 'down' was pressed." << std::endl;
        return new DuckingState();
      }

      return NULL;
    }


    virtual void enter(Character& character) {
      character.setGraphics();
    }

};

class DuckingState : public CharacterState
{
  public:
    DuckingState()
    : time_(0)
    {}

    virtual CharacterState* handleInput(Character& character, Input input) {
      if (input == RELEASE_DOWN)
      {
        std::cout << "I was ducking and then 'down' was released." << std::endl;
        return new StandingState();
      }

      return NULL;
    }

    virtual void update(Character& character) {
      time_++;
      if (time_ > MAX_CHARGE)
      {
        character.superAttack();
      }
    }

  private:
    int time_;
};