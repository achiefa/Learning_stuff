#include <iostream>

#define MAX_CHARGE 10

//------------ INPUT ------------
enum Inputs
{
  RELEASE_DOWN,
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
      state_->handleInput(*this, input);
    }

    virtual void update()
    {
      state_->update(*this);
    }

    void setState(CharacterState* state) { 
      state_ = state;
    }

    virtual void superAttack() {
      std::cout << "Standard super attack" << std::endl;
    }

  private:
    CharacterState* state_;
  
  friend class CharacterState;
};

//------------ CHARACTER STATES static ------------
class CharacterState // State interface
{
  public:
    virtual ~CharacterState() {}
    virtual void handleInput(Character& character, Input inpput);
    virtual void update(Character& character);

    static DuckingState ducking;
};


class DuckingState : public CharacterState
{
  public:
    DuckingState()
    : time_(0)
    {}

    virtual void handleInput(Character& character, Input input) {
      if (input == RELEASE_DOWN)
      {
        character.setState(&CharacterState::ducking);
      }
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