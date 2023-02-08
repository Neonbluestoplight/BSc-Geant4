/// Action initialization class.

/// \file ActionInitialization.hh
/// \brief Definition of the B4c::ActionInitialization class

#ifndef B4cActionInitialization_h
#define B4cActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

namespace B4c
{
class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    ~ActionInitialization() override;

    void BuildForMaster() const override;
    void Build() const override;
};

}

#endif


