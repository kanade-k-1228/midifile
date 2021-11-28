//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Jul 26 13:10:22 PDT 2010
// Last Modified: Mon Feb  9 20:34:40 PST 2015 Updated for C++11.
// Filename:      ...sig/doc/examples/all/miditime/miditime.cpp
// Syntax:        C++
//
// Description:   Displays the absolute tick time and absolute time
//                in seconds for MIDI events in a MIDI file, along with
//                the track information.

#include "MidiFile.h"
#include "Options.h"
#include <cmath>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace smf;

// function declarations:
void checkOptions(Options &opts);
void example(void);
void usage(const char *command);
void processMidiFile(MidiFile &midifile);

// user interface variables:
double starttime = 0.0; // used with -s option
double endtime = 0.0;   // used with -e option
int onsetQ = 0;         // used with -o option

///////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  int status;
  MidiFile inputfile;
  Options options(argc, argv);

  checkOptions(options);

  status = inputfile.read(options.getArg(1));
  if (status == 0) {
    cout << "Syntax error in file: " << options.getArg(1) << "\n";
  }

  processMidiFile(inputfile);
  return 0;
}

///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile -- extract track and timing information from the MIDI file.
//

void processMidiFile(MidiFile &midifile) {
  midifile.absoluteTicks();
  midifile.joinTracks();
  int eventcount = midifile.getEventCount(0);
  MidiEvent *ptr;

  // Interval Time Var
  int prev_time = 0;
  int interval_time = 0;
  int count = 0;

  cout << "void loop(){" << endl;

  for (int i = 0; i < eventcount; i++) {
    ptr = &(midifile[0][i]);
    int track = ptr->track;
    //  int timeinticks = ptr->tick;
    double timeinsecs = midifile.getTimeInSeconds(0, i);
    int abs_time = round(timeinsecs * 1000);
    int attack = ((*ptr)[0] & 0xf0) == 0x90;
    if (onsetQ && !attack) {
      continue;
    }
    if (onsetQ && attack) {
      if ((*ptr)[2] == 0) {
        continue;
      }
    }

    // Select 0x8? or 0x9?
    int command_nibble = (int)(*ptr)[0];
    if (0x80 <= command_nibble && command_nibble < 0xA0) {
      // Set Interval Time
      interval_time = abs_time - prev_time;
      prev_time = abs_time;
      if (interval_time != 0) {
        cout << "delay(" << interval_time << ");" << endl;
        count++;
      }
      int tone_number = (int)(*ptr)[1];
      int volume = (int)(*ptr)[2];
      cout << "play_sound(" << track << "," << tone_number << "," << volume
           << ");" << endl;
      count++;
    }
  }
  cout << "}" << endl;
}

//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options &opts) {

  opts.define("author=b");
  opts.define("version=b");
  opts.define("example=b");
  opts.define("help=b");
  opts.define("o|on|onset=b");
  opts.process();

  if (opts.getBoolean("author")) {
    cout << "Written by Craig Stuart Sapp, "
         << "craig@ccrma.stanford.edu, July 2010" << endl;
    exit(0);
  }
  if (opts.getBoolean("version")) {
    cout << "midiextract version 1.0" << endl;
    cout << "compiled: " << __DATE__ << endl;
  }
  if (opts.getBoolean("help")) {
    usage(opts.getCommand().c_str());
    exit(0);
  }
  if (opts.getBoolean("example")) {
    example();
    exit(0);
  }

  onsetQ = opts.getBoolean("onset");
}

//////////////////////////////
//
// example -- gives example calls to the midiexcerpt program.
//

void example(void) {
  cout << "#                                                                   "
          "     \n"
       << endl;
}

//////////////////////////////
//
// usage -- how to run the midiexcerpt program on the command line.
//

void usage(const char *command) {
  cout << "                                                                    "
          "     \n"
       << endl;
}
