/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2022, Live Networks, Inc.  All rights reserved
// A subclass of "RTSPServer" that creates "ServerMediaSession"s on demand,
// based on whether or not the specified stream name exists as a file
// Implementation

#include "DynamicRTSPServer.hh"
#include <liveMedia.hh>
#include <string.h>

#include "H264FramedLiveSource.hh"
#include "H264LiveVideoServerMediaSubssion.hh"
#include "H265LiveVideoServerMediaSubssion.hh"

DynamicRTSPServer*
DynamicRTSPServer::createNew(UsageEnvironment& env, Port ourPort,
			     UserAuthenticationDatabase* authDatabase,
			     Boolean h265,
			     char* streamName,
			     unsigned reclamationTestSeconds) {
  int ourSocketIPv4 = setUpOurSocket(env, ourPort, AF_INET);
  int ourSocketIPv6 = setUpOurSocket(env, ourPort, AF_INET6);
  if (ourSocketIPv4 < 0 && ourSocketIPv6 < 0) return NULL;

  return new DynamicRTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort,
			       authDatabase, reclamationTestSeconds, h265, streamName);
}

DynamicRTSPServer::DynamicRTSPServer(UsageEnvironment& env, int ourSocketIPv4, int ourSocketIPv6,
				     Port ourPort,
				     UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds,
				     Boolean h265,
				     char* streamName)
  : RTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationTestSeconds)
  , _h265(h265)
  , _streamName(streamName) {

  printf("[MEDIA SERVER]Dynamic RTSP server constructed\n");
}

DynamicRTSPServer::~DynamicRTSPServer() {
  printf("[MEDIA SERVER]Dynamic RTSP server desctructed\n");
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* fid, DynamicRTSPServer* server); // forward

void DynamicRTSPServer
::lookupServerMediaSession(char const* streamName,
			   lookupServerMediaSessionCompletionFunc* completionFunc,
			   void* completionClientData,
			   Boolean isFirstLookupInSession) {
  // First, check whether the specified "streamName" exists as a local file:
  FILE* fid = fopen(streamName, "rb");
  Boolean const fileExists = fid != NULL;
  Boolean isStream = (strcmp(streamName, _streamName) == 0);

  // Next, check whether we already have a "ServerMediaSession" for this file:
  ServerMediaSession* sms = getServerMediaSession(streamName);
  Boolean const smsExists = (sms != NULL);

  printf("[MEDIA SERVER]Dynamic RTSP server lookup, name=%s\n", streamName);
  printf("[MEDIA SERVER]Dynamic RTSP server lookup, smsExists=%d, isStream=%d, fileExists=%d\n", smsExists, isStream, fileExists);

  // Handle the four possibilities for "fileExists" and "smsExists":
  if (!fileExists && !isStream) {
    if (smsExists) {
      // "sms" was created for a file that no longer exists. Remove it:
      removeServerMediaSession(sms);
    }

    sms = NULL;
  } else {
    if (smsExists && isFirstLookupInSession) {
      // Remove the existing "ServerMediaSession" and create a new one, in case the underlying
      // file has changed in some way:
      printf("[MEDIA SERVER]Server Media Session %p remove, refCount=%d.\n", sms, sms->referenceCount());
      removeServerMediaSession(sms);
      sms = NULL;
    }

    if (sms == NULL) {
      sms = createNewSMS(envir(), streamName, fid, this);
      if( sms != NULL ) {
        addServerMediaSession(sms);
        printf("[MEDIA SERVER]Server Media Session %p added.\n", sms);
      }
      else {
        printf("[MEDIA SERVER]Server Media Session create failed.\n");
      }
    }

    if(fileExists) {
        fclose(fid);
    }
  }

  if (completionFunc != NULL) {
    (*completionFunc)(completionClientData, sms);
  }
}

#define NEW_SMS(description) do {\
char const* descStr = description\
    ", streamed by the LIVE555 Media Server";\
sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);\
} while(0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* fid, DynamicRTSPServer* server) {
  // Use the file name extension to determine the type of "ServerMediaSession":
  char const* extension = strrchr(fileName, '.');
  Boolean isStream = (fid == NULL);
  if ((extension == NULL) && !isStream) return NULL;

  ServerMediaSession* sms = NULL;
  Boolean const reuseSource = True;
  if (isStream) {
    // live stream
    // First, make sure that the RTPSinks' buffers will be large enough to handle the huge size of DV frames (as big as 288000).
    OutPacketBuffer::maxSize = 2*1024*1024;

    NEW_SMS("Live Stream");

    if( server->_h265 ) {
        sms->addSubsession(H265LiveVideoServerMediaSubssion::createNew(env, reuseSource));
    }
    else {
        sms->addSubsession(H264LiveVideoServerMediaSubssion::createNew(env, reuseSource));
    }
  }

  return sms;
}
