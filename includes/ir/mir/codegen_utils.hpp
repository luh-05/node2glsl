#pragma once
#define GET_CONSTANT(Name, Var, Context)                                       \
  if (auto status = Context->GetConstant<const typeof(Var)>(this, Name);       \
      !status.ok())                                                            \
    return status.status();                                                    \
  else                                                                         \
    Var = status.value()
