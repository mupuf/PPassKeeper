CXXFLAGS = `pkg-config ppasskeeper --cflags`
LDFLAGS = `pkg-config ppasskeeper --libs`

.SUFFIXES: .cpp

SOURCES = $(wildcard *.cpp)
TARGETS = $(SOURCES:%.cpp=%)

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
