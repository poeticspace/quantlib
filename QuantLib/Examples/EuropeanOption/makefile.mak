
# makefile for EuropeanOption.cpp under Borland C++

.autodepend
.silent

# Debug version
!ifdef DEBUG
    _D = _d
!endif


# Directories
QL_INCLUDE_DIR   = "$(QL_DIR)"
QL_LIB_DIR       = "$(QL_DIR)\lib\Win32\Borland"
BCC_INCLUDE      = $(MAKEDIR)\..\include
BCC_LIBS         = $(MAKEDIR)\..\lib
SRCDIR           = "."
OBJDIR           = ".\build\Borland"

#Warning W8057 : Parameter 'argc' is never used in function main(int,char * *)

# Options
CC_OPTS = -vi- -w-8057 \
    -I$(QL_INCLUDE_DIR) \
    -I$(BCC_INCLUDE) \
    -n$(OBJDIR)

!ifdef DEBUG
CC_OPTS = $(CC_OPTS) -v -DQL_DEBUG
!endif
!ifdef SAFE
CC_OPTS = $(CC_OPTS) -DQL_EXTRA_SAFETY_CHECKS
!endif

# Primary target:
EuropeanOption$(_D).exe: $(OBJDIR) EuropeanOption.cpp
    bcc32 $(CC_OPTS) -L$(QL_LIB_DIR) -L$(BCC_LIBS) \
    -o"$(OBJDIR)\EuropeanOption$(_D).obj" -eEuropeanOption$(_D).exe \
    EuropeanOption.cpp QuantLib$(_D).lib

#create build dir
$(OBJDIR):
        @if not exist $(OBJDIR) (md $(OBJDIR))

# Clean up
clean::
    if exist $(OBJDIR)\*.obj   del /q $(OBJDIR)\*.obj
    if exist $(OBJDIR)\*.tds   del /q $(OBJDIR)\*.tds
    if exist $(OBJDIR)\*.exe   del /q $(OBJDIR)\*.exe
