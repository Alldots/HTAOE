#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hacking.h"

#define FILENAME "/var/notes"

int print_notes(int, int, char *);  // Note printing function
int find_user_note(int, int);	    // Seek in file for a note for user
int search_note(char *, char*);     // Search for keyword function.
void fatal(char *);		    // Fatal error handler

int main(int argc, char *argv[]) {
  int userid, printing, fd; // File descriptor
  char searchstring[100];

  if(argc > 1)				// If there is an arg,
    strcpy(searchstring, argv[1]);	//   that is the search string
  else					// Otherwise
    searchstring[0] = 0;		//   search string is empty.
  
  userid = getuid();
  fd = open(FILENAME, O_RDONLY);   // Open the file for read-only access
  if(fd == -1)
    fatal("in main() while opening file");
  
  while(printing)
    printing = print_notes(fd, userid, searchstring);
  printf("-------[ end of note date]-------\n");
  close(fd);
}

// A function to print the notes for a given uid that match
// an optional search string;
// returns 0 at end of file, 1 if there are still some more notes
int print_notes(int fd, int uid, char *searchstring) {
  int note_length;
  char note_buffer[100];

  note_length = find_user_note(fd, uid);
  if(note_length == -1)
    return 0;

  read(fd, note_buffer, note_length);  // Read note data
  note_buffer[note_length] = 0;	// Terminate the note buffer

  if(search_note(note_buffer, searchstring)) // if search string found
    printf(note_buffer);	// print the note
  return 1;
}

// A function to search for the next note for a given user ID
// returns -1 if the end of file reached
// Otherwise returns the length of the found note
int find_user_note(int fd, int user_uid) {
  int note_uid = -1;
  unsigned char byte;
  int length;

  while(note_uid != user_uid) {
    if(read(fd, &note_uid, 4) != 4) // Read the uid data
      return -1;
    if(read(fd, &byte, 1) != 1) // Read the newline seperator.
      return -1;
   
    byte = length = 0;
    while(byte != '\n') { // Figure out how many bytes to the end of line
      if(read(fd, &byte, 1) != 1)  // Read a single byte,
        return -1;		   // If bytes isn't read, return end of file code.
      length++;
    }
  }  
  lseek(fd, length * -1, SEEK_CUR); // Rewind file reading by length bytes.

  printf("[DEBUG] found %d bytes note for userid %d\n", length, user_uid);
  return length;
}

// A function to search note for a given keyword;
// returns 1 if a match is found, 0 if there is no match.
int search_note(char *note, char *keyword) {
  int i, keyword_length, note_length,  match=0;

  keyword_length = strlen(keyword);
  if(keyword_length == 0) // If there is no search string
    return 1;		  // Always match

  note_length = strlen(note);
  for(i=0; i < note_length; i++) { // Iterate over bytes in note.
    if(note[i] == keyword[match]) // If byte matches keyword,
      match++;	// get ready to match next byte.
    else {
      if(note[i] == keyword[0]) // if that byte matches first keyword byte,
        match = 1;              // start the match with count at 1
      else
        match = 0;             // otherwise its zero
    }
    if(match==keyword_length) //If there is a full match,
      return 1;  // return matched
  }

  return 0; // return not matched
}
  
 
    


