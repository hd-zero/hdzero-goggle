/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * MediaPlayer class can be used to control playback
 * of audio/video files and streams. An example on how to use the methods in
 * this class can be found in {@link android.widget.VideoView}.
 *
 * <p>Topics covered here are:
 * <ol>
 * <li><a href="#StateDiagram">State Diagram</a>
 * <li><a href="#Valid_and_Invalid_States">Valid and Invalid States</a>
 * <li><a href="#Permissions">Permissions</a>
 * <li><a href="#Callbacks">Register informational and error callbacks</a>
 * </ol>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about how to use MediaPlayer, read the
 * <a href="{@docRoot}guide/topics/media/mediaplayer.html">Media Playback</a> developer guide.</p>
 * </div>
 *
 * <a name="StateDiagram"></a>
 * <h3>State Diagram</h3>
 *
 * <p>Playback control of audio/video files and streams is managed as a state
 * machine. The following diagram shows the life cycle and the states of a
 * MediaPlayer object driven by the supported playback control operations.
 * The ovals represent the states a MediaPlayer object may reside
 * in. The arcs represent the playback control operations that drive the object
 * state transition. There are two types of arcs. The arcs with a single arrow
 * head represent synchronous method calls, while those with
 * a double arrow head represent asynchronous method calls.</p>
 *
 * <p><img src="../../../images/mediaplayer_state_diagram.gif"
 *         alt="MediaPlayer State diagram"
 *         border="0" /></p>
 *
 * <p>From this state diagram, one can see that a MediaPlayer object has the
 *    following states:</p>
 * <ul>
 *     <li>When a MediaPlayer object is just created using <code>new</code> or
 *         after {@link #reset()} is called, it is in the <em>Idle</em> state; and after
 *         {@link #release()} is called, it is in the <em>End</em> state. Between these
 *         two states is the life cycle of the MediaPlayer object.
 *         <ul>
 *         <li>There is a subtle but important difference between a newly constructed
 *         MediaPlayer object and the MediaPlayer object after {@link #reset()}
 *         is called. It is a programming error to invoke methods such
 *         as {@link #getCurrentPosition()},
 *         {@link #getDuration()}, {@link #getVideoHeight()},
 *         {@link #getVideoWidth()}, {@link #setAudioStreamType(int)},
 *         {@link #setLooping(boolean)},
 *         {@link #setVolume(float, float)}, {@link #pause()}, {@link #start()},
 *         {@link #stop()}, {@link #seekTo(int)}, {@link #prepare()} or
 *         {@link #prepareAsync()} in the <em>Idle</em> state for both cases. If any of these
 *         methods is called right after a MediaPlayer object is constructed,
 *         the user supplied callback method OnErrorListener.onError() won't be
 *         called by the internal player engine and the object state remains
 *         unchanged; but if these methods are called right after {@link #reset()},
 *         the user supplied callback method OnErrorListener.onError() will be
 *         invoked by the internal player engine and the object will be
 *         transfered to the <em>Error</em> state. </li>
 *         <li>It is also recommended that once
 *         a MediaPlayer object is no longer being used, call {@link #release()} immediately
 *         so that resources used by the internal player engine associated with the
 *         MediaPlayer object can be released immediately. Resource may include
 *         singleton resources such as hardware acceleration components and
 *         failure to call {@link #release()} may cause subsequent instances of
 *         MediaPlayer objects to fallback to software implementations or fail
 *         altogether. Once the MediaPlayer
 *         object is in the <em>End</em> state, it can no longer be used and
 *         there is no way to bring it back to any other state. </li>
 *         <li>Furthermore,
 *         the MediaPlayer objects created using <code>new</code> is in the
 *         <em>Idle</em> state, while those created with one
 *         of the overloaded convenient <code>create</code> methods are <em>NOT</em>
 *         in the <em>Idle</em> state. In fact, the objects are in the <em>Prepared</em>
 *         state if the creation using <code>create</code> method is successful.
 *         </li>
 *         </ul>
 *         </li>
 *     <li>In general, some playback control operation may fail due to various
 *         reasons, such as unsupported audio/video format, poorly interleaved
 *         audio/video, resolution too high, streaming timeout, and the like.
 *         Thus, error reporting and recovery is an important concern under
 *         these circumstances. Sometimes, due to programming errors, invoking a playback
 *         control operation in an invalid state may also occur. Under all these
 *         error conditions, the internal player engine invokes a user supplied
 *         OnErrorListener.onError() method if an OnErrorListener has been
 *         registered beforehand via
 *         {@link #setOnErrorListener(android.media.MediaPlayer.OnErrorListener)}.
 *         <ul>
 *         <li>It is important to note that once an error occurs, the
 *         MediaPlayer object enters the <em>Error</em> state (except as noted
 *         above), even if an error listener has not been registered by the application.</li>
 *         <li>In order to reuse a MediaPlayer object that is in the <em>
 *         Error</em> state and recover from the error,
 *         {@link #reset()} can be called to restore the object to its <em>Idle</em>
 *         state.</li>
 *         <li>It is good programming practice to have your application
 *         register a OnErrorListener to look out for error notifications from
 *         the internal player engine.</li>
 *         <li>IllegalStateException is
 *         thrown to prevent programming errors such as calling {@link #prepare()},
 *         {@link #prepareAsync()}, or one of the overloaded <code>setDataSource
 *         </code> methods in an invalid state. </li>
 *         </ul>
 *         </li>
 *     <li>Calling
 *         {@link #setDataSource(FileDescriptor)}, or
 *         {@link #setDataSource(String)}, or
 *         {@link #setDataSource(Context, Uri)}, or
 *         {@link #setDataSource(FileDescriptor, long, long)} transfers a
 *         MediaPlayer object in the <em>Idle</em> state to the
 *         <em>Initialized</em> state.
 *         <ul>
 *         <li>An IllegalStateException is thrown if
 *         setDataSource() is called in any other state.</li>
 *         <li>It is good programming
 *         practice to always look out for <code>IllegalArgumentException</code>
 *         and <code>IOException</code> that may be thrown from the overloaded
 *         <code>setDataSource</code> methods.</li>
 *         </ul>
 *         </li>
 *     <li>A MediaPlayer object must first enter the <em>Prepared</em> state
 *         before playback can be started.
 *         <ul>
 *         <li>There are two ways (synchronous vs.
 *         asynchronous) that the <em>Prepared</em> state can be reached:
 *         either a call to {@link #prepare()} (synchronous) which
 *         transfers the object to the <em>Prepared</em> state once the method call
 *         returns, or a call to {@link #prepareAsync()} (asynchronous) which
 *         first transfers the object to the <em>Preparing</em> state after the
 *         call returns (which occurs almost right way) while the internal
 *         player engine continues working on the rest of preparation work
 *         until the preparation work completes. When the preparation completes or when {@link #prepare()} call returns,
 *         the internal player engine then calls a user supplied callback method,
 *         onPrepared() of the OnPreparedListener interface, if an
 *         OnPreparedListener is registered beforehand via {@link
 *         #setOnPreparedListener(android.media.MediaPlayer.OnPreparedListener)}.</li>
 *         <li>It is important to note that
 *         the <em>Preparing</em> state is a transient state, and the behavior
 *         of calling any method with side effect while a MediaPlayer object is
 *         in the <em>Preparing</em> state is undefined.</li>
 *         <li>An IllegalStateException is
 *         thrown if {@link #prepare()} or {@link #prepareAsync()} is called in
 *         any other state.</li>
 *         <li>While in the <em>Prepared</em> state, properties
 *         such as audio/sound volume, screenOnWhilePlaying, looping can be
 *         adjusted by invoking the corresponding set methods.</li>
 *         </ul>
 *         </li>
 *     <li>To start the playback, {@link #start()} must be called. After
 *         {@link #start()} returns successfully, the MediaPlayer object is in the
 *         <em>Started</em> state. {@link #isPlaying()} can be called to test
 *         whether the MediaPlayer object is in the <em>Started</em> state.
 *         <ul>
 *         <li>While in the <em>Started</em> state, the internal player engine calls
 *         a user supplied OnBufferingUpdateListener.onBufferingUpdate() callback
 *         method if a OnBufferingUpdateListener has been registered beforehand
 *         via {@link #setOnBufferingUpdateListener(OnBufferingUpdateListener)}.
 *         This callback allows applications to keep track of the buffering status
 *         while streaming audio/video.</li>
 *         <li>Calling {@link #start()} has not effect
 *         on a MediaPlayer object that is already in the <em>Started</em> state.</li>
 *         </ul>
 *         </li>
 *     <li>Playback can be paused and stopped, and the current playback position
 *         can be adjusted. Playback can be paused via {@link #pause()}. When the call to
 *         {@link #pause()} returns, the MediaPlayer object enters the
 *         <em>Paused</em> state. Note that the transition from the <em>Started</em>
 *         state to the <em>Paused</em> state and vice versa happens
 *         asynchronously in the player engine. It may take some time before
 *         the state is updated in calls to {@link #isPlaying()}, and it can be
 *         a number of seconds in the case of streamed content.
 *         <ul>
 *         <li>Calling {@link #start()} to resume playback for a paused
 *         MediaPlayer object, and the resumed playback
 *         position is the same as where it was paused. When the call to
 *         {@link #start()} returns, the paused MediaPlayer object goes back to
 *         the <em>Started</em> state.</li>
 *         <li>Calling {@link #pause()} has no effect on
 *         a MediaPlayer object that is already in the <em>Paused</em> state.</li>
 *         </ul>
 *         </li>
 *     <li>Calling  {@link #stop()} stops playback and causes a
 *         MediaPlayer in the <em>Started</em>, <em>Paused</em>, <em>Prepared
 *         </em> or <em>PlaybackCompleted</em> state to enter the
 *         <em>Stopped</em> state.
 *         <ul>
 *         <li>Once in the <em>Stopped</em> state, playback cannot be started
 *         until {@link #prepare()} or {@link #prepareAsync()} are called to set
 *         the MediaPlayer object to the <em>Prepared</em> state again.</li>
 *         <li>Calling {@link #stop()} has no effect on a MediaPlayer
 *         object that is already in the <em>Stopped</em> state.</li>
 *         </ul>
 *         </li>
 *     <li>The playback position can be adjusted with a call to
 *         {@link #seekTo(int)}.
 *         <ul>
 *         <li>Although the asynchronuous {@link #seekTo(int)}
 *         call returns right way, the actual seek operation may take a while to
 *         finish, especially for audio/video being streamed. When the actual
 *         seek operation completes, the internal player engine calls a user
 *         supplied OnSeekComplete.onSeekComplete() if an OnSeekCompleteListener
 *         has been registered beforehand via
 *         {@link #setOnSeekCompleteListener(OnSeekCompleteListener)}.</li>
 *         <li>Please
 *         note that {@link #seekTo(int)} can also be called in the other states,
 *         such as <em>Prepared</em>, <em>Paused</em> and <em>PlaybackCompleted
 *         </em> state.</li>
 *         <li>Furthermore, the actual current playback position
 *         can be retrieved with a call to {@link #getCurrentPosition()}, which
 *         is helpful for applications such as a Music player that need to keep
 *         track of the playback progress.</li>
 *         </ul>
 *         </li>
 *     <li>When the playback reaches the end of stream, the playback completes.
 *         <ul>
 *         <li>If the looping mode was being set to <var>true</var>with
 *         {@link #setLooping(boolean)}, the MediaPlayer object shall remain in
 *         the <em>Started</em> state.</li>
 *         <li>If the looping mode was set to <var>false
 *         </var>, the player engine calls a user supplied callback method,
 *         OnCompletion.onCompletion(), if a OnCompletionListener is registered
 *         beforehand via {@link #setOnCompletionListener(OnCompletionListener)}.
 *         The invoke of the callback signals that the object is now in the <em>
 *         PlaybackCompleted</em> state.</li>
 *         <li>While in the <em>PlaybackCompleted</em>
 *         state, calling {@link #start()} can restart the playback from the
 *         beginning of the audio/video source.</li>
 * </ul>
 *
 *
 * <a name="Valid_and_Invalid_States"></a>
 * <h3>Valid and invalid states</h3>
 *
 * <table border="0" cellspacing="0" cellpadding="0">
 * <tr><td>Method Name </p></td>
 *     <td>Valid Sates </p></td>
 *     <td>Invalid States </p></td>
 *     <td>Comments </p></td></tr>
 * <tr><td>attachAuxEffect </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted} </p></td>
 *     <td>{Idle, Error} </p></td>
 *     <td>This method must be called after setDataSource.
 *     Calling it does not change the object state. </p></td></tr>
 * <tr><td>getAudioSessionId </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>getCurrentPosition </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted} </p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change the
 *         state. Calling this method in an invalid state transfers the object
 *         to the <em>Error</em> state. </p></td></tr>
 * <tr><td>getDuration </p></td>
 *     <td>{Prepared, Started, Paused, Stopped, PlaybackCompleted} </p></td>
 *     <td>{Idle, Initialized, Error} </p></td>
 *     <td>Successful invoke of this method in a valid state does not change the
 *         state. Calling this method in an invalid state transfers the object
 *         to the <em>Error</em> state. </p></td></tr>
 * <tr><td>getVideoHeight </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change the
 *         state. Calling this method in an invalid state transfers the object
 *         to the <em>Error</em> state.  </p></td></tr>
 * <tr><td>getVideoWidth </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an invalid state transfers the
 *         object to the <em>Error</em> state. </p></td></tr>
 * <tr><td>isPlaying </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *          PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an invalid state transfers the
 *         object to the <em>Error</em> state. </p></td></tr>
 * <tr><td>pause </p></td>
 *     <td>{Started, Paused}</p></td>
 *     <td>{Idle, Initialized, Prepared, Stopped, PlaybackCompleted, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Paused</em> state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>prepare </p></td>
 *     <td>{Initialized, Stopped} </p></td>
 *     <td>{Idle, Prepared, Started, Paused, PlaybackCompleted, Error} </p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Prepared</em> state. Calling this method in an
 *         invalid state throws an IllegalStateException.</p></td></tr>
 * <tr><td>prepareAsync </p></td>
 *     <td>{Initialized, Stopped} </p></td>
 *     <td>{Idle, Prepared, Started, Paused, PlaybackCompleted, Error} </p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Preparing</em> state. Calling this method in an
 *         invalid state throws an IllegalStateException.</p></td></tr>
 * <tr><td>release </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>After {@link #release()}, the object is no longer available. </p></td></tr>
 * <tr><td>reset </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted, Error}</p></td>
 *     <td>{}</p></td>
 *     <td>After {@link #reset()}, the object is like being just created.</p></td></tr>
 * <tr><td>seekTo </p></td>
 *     <td>{Prepared, Started, Paused, PlaybackCompleted} </p></td>
 *     <td>{Idle, Initialized, Stopped, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an invalid state transfers the
 *         object to the <em>Error</em> state. </p></td></tr>
 * <tr><td>setAudioSessionId </p></td>
 *     <td>{Idle} </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted,
 *          Error} </p></td>
 *     <td>This method must be called in idle state as the audio session ID must be known before
 *         calling setDataSource. Calling it does not change the object state. </p></td></tr>
 * <tr><td>setAudioStreamType </p></td>
 *     <td>{Idle, Initialized, Stopped, Prepared, Started, Paused,
 *          PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method does not change the state. In order for the
 *         target audio stream type to become effective, this method must be called before
 *         prepare() or prepareAsync().</p></td></tr>
 * <tr><td>setAuxEffectSendLevel </p></td>
 *     <td>any</p></td>
 *     <td>{} </p></td>
 *     <td>Calling this method does not change the object state. </p></td></tr>
 * <tr><td>setDataSource </p></td>
 *     <td>{Idle} </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted,
 *          Error} </p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Initialized</em> state. Calling this method in an
 *         invalid state throws an IllegalStateException.</p></td></tr>
 * <tr><td>setDisplay </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setSurface </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setVideoScalingMode </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted} </p></td>
 *     <td>{Idle, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>setLooping </p></td>
 *     <td>{Idle, Initialized, Stopped, Prepared, Started, Paused,
 *         PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>isLooping </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnBufferingUpdateListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnCompletionListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnErrorListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnPreparedListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnSeekCompleteListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setScreenOnWhilePlaying</></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state.  </p></td></tr>
 * <tr><td>setVolume </p></td>
 *     <td>{Idle, Initialized, Stopped, Prepared, Started, Paused,
 *          PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.
 * <tr><td>setWakeMode </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state.</p></td></tr>
 * <tr><td>start </p></td>
 *     <td>{Prepared, Started, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Stopped, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Started</em> state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>stop </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Stopped</em> state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>getTrackInfo </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>addTimedTextSource </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>selectTrack </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>deselectTrack </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 *
 * </table>
 *
 * <a name="Permissions"></a>
 * <h3>Permissions</h3>
 * <p>One may need to declare a corresponding WAKE_LOCK permission {@link
 * android.R.styleable#AndroidManifestUsesPermission &lt;uses-permission&gt;}
 * element.
 *
 * <p>This class requires the {@link android.Manifest.permission#INTERNET} permission
 * when used with network-based content.
 *
 * <a name="Callbacks"></a>
 * <h3>Callbacks</h3>
 * <p>Applications may want to register for informational and error
 * events in order to be informed of some internal state update and
 * possible runtime errors during playback or streaming. Registration for
 * these events is done by properly setting the appropriate listeners (via calls
 * to
 * {@link #setOnPreparedListener(OnPreparedListener)}setOnPreparedListener,
 * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}setOnVideoSizeChangedListener,
 * {@link #setOnSeekCompleteListener(OnSeekCompleteListener)}setOnSeekCompleteListener,
 * {@link #setOnCompletionListener(OnCompletionListener)}setOnCompletionListener,
 * {@link #setOnBufferingUpdateListener(OnBufferingUpdateListener)}setOnBufferingUpdateListener,
 * {@link #setOnInfoListener(OnInfoListener)}setOnInfoListener,
 * {@link #setOnErrorListener(OnErrorListener)}setOnErrorListener, etc).
 * In order to receive the respective callback
 * associated with these listeners, applications are required to create
 * MediaPlayer objects on a thread with its own Looper running (main UI
 * thread by default has a Looper running).
 *
 */
#ifndef __IPCLINUX_EYESEEPLAYER_H__
#define __IPCLINUX_EYESEEPLAYER_H__

#include <string>
//#include <utils/KeyedVector.h>
//#include <utils/String8.h>
//#include <binder/Parcel.h>

//#include <binder/IMemory.h>
//#include <binder/MemoryBase.h>
//#include <binder/MemoryHeapBase.h>

//#include <CedarMetaData.h>
#include <CallbackDispatcher.h>
//#include <media/mediaplayer.h>
#include <EyeseeMessageQueue.h>

#include <mm_common.h>
#include <mm_comm_demux.h>
#include <mm_comm_vdec.h>
#include <mm_comm_adec.h>
#include <mm_comm_vo.h>
#include <mm_comm_aio.h>
#include <mm_comm_clock.h>

namespace EyeseeLinux {

enum media_player_states {
    MEDIA_PLAYER_STATE_ERROR        = 0,
    MEDIA_PLAYER_IDLE               = 1 << 0,
    MEDIA_PLAYER_INITIALIZED        = 1 << 1,
    MEDIA_PLAYER_PREPARING          = 1 << 2,
    MEDIA_PLAYER_PREPARED           = 1 << 3,
    MEDIA_PLAYER_STARTED            = 1 << 4,
    MEDIA_PLAYER_PAUSED             = 1 << 5,
    MEDIA_PLAYER_STOPPED            = 1 << 6,
    MEDIA_PLAYER_PLAYBACK_COMPLETE  = 1 << 7
};

class EyeseePlayer
{
private:
    VO_LAYER mSurfaceHolder;
public:
    /**
     * Default constructor. Consider using one of the create() methods for
     * synchronously instantiating a MediaPlayer from a Uri or resource.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances may
     * result in an exception.</p>
     */
    EyeseePlayer();
    ~EyeseePlayer();


    /* Do not change these values (starting with INVOKE_ID) without updating
     * their counterparts in include/media/mediaplayer.h!
     */
private:
    enum
    {
        INVOKE_ID_GET_TRACK_INFO = 1,
        INVOKE_ID_ADD_EXTERNAL_SOURCE = 2,
        INVOKE_ID_ADD_EXTERNAL_SOURCE_FD = 3,
        INVOKE_ID_SELECT_TRACK = 4,
        INVOKE_ID_DESELECT_TRACK = 5,
        INVOKE_ID_SET_VIDEO_SCALE_MODE = 6,
    };

public:

    /**
     * Sets the {@link SurfaceHolder} to use for displaying the video
     * portion of the media.
     *
     * Either a surface holder or surface must be set if a display or video sink
     * is needed.  Not calling this method or {@link #setSurface(Surface)}
     * when playing back a video will result in only the audio track being played.
     * A null surface holder or surface will result in only the audio track being
     * played.
     *
     * @param sh the SurfaceHolder to use for video display
     */
    void setDisplay(unsigned int hlay);

    /**
     * Set output pixel format. must call before {@link #prepare()}
     *
     * @param ePixelFormat the pixel format user want to output.
     */
    status_t setOutputPixelFormat(PIXEL_FORMAT_E ePixelFormat);    //PIXEL_FORMAT_YVU_PLANAR_420

    /**
     * Sets video scaling mode. To make the target video scaling mode
     * effective during playback, this method must be called after
     * data source is set. If not called, the default video
     * scaling mode is {@link #VIDEO_SCALING_MODE_SCALE_TO_FIT}.
     *
     * <p> The supported video scaling modes are:
     * <ul>
     * <li> {@link #VIDEO_SCALING_MODE_SCALE_TO_FIT}
     * <li> {@link #VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING}
     * </ul>
     *
     * @param mode target video scaling mode. Most be one of the supported
     * video scaling modes; otherwise, IllegalArgumentException will be thrown.
     *
     * @see NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW
     * @see NATIVE_WINDOW_SCALING_MODE_SCALE_CROP
     */
    status_t setVideoScalingMode(int mode);

    /**
     * Sets the data source (file-path or http/rtsp URL) to use.
     *
     * @param path the path of the file, or the http/rtsp URL of the stream you want to play
     * @throws IllegalStateException if it is called in an invalid state
     *
     * <p>When <code>path</code> refers to a local file, the file may actually be opened by a
     * process other than the calling application.  This implies that the pathname
     * should be an absolute path (as any other process runs with unspecified current working
     * directory), and that the pathname should reference a world-readable file.
     * As an alternative, the application could first open the file for reading,
     * and then use the file descriptor form {@link #setDataSource(FileDescriptor)}.
     */
    status_t setDataSource(std::string path);

    /**
     * Sets the data source (FileDescriptor) to use. It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @throws IllegalStateException if it is called in an invalid state
     */
    status_t setDataSource(int fd);

    /**
     * Sets the data source (FileDescriptor) to use.  The FileDescriptor must be
     * seekable (N.B. a LocalSocket is not seekable). It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param offset the offset into the file where the data to be played starts, in bytes
     * @param length the length in bytes of the data to be played
     * @throws IllegalStateException if it is called in an invalid state
     */
    status_t setDataSource(int fd, int64_t offset, int64_t length);


    status_t setVdecInputBufferSize(unsigned int nBufferSize);
    unsigned int getVdecInputBufferSize() const ;

    /**
     * Prepares the player for playback, synchronously.
     *
     * After setting the datasource and the display surface, you need to either
     * call prepare() or prepareAsync(). For files, it is OK to call prepare(),
     * which blocks until MediaPlayer is ready for playback.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    status_t prepare();

    /**
     * Prepares the player for playback, asynchronously.
     *
     * After setting the datasource and the display surface, you need to either
     * call prepare() or prepareAsync(). For streams, you should call prepareAsync(),
     * which returns immediately, rather than blocking until enough data has been
     * buffered.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    //status_t prepareAsync();

    /**
     * Starts or resumes playback. If playback had previously been paused,
     * playback will continue from where it was paused. If playback had
     * been stopped, or never started before, playback will start at the
     * beginning.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    status_t start();

    /**
     * Stops playback after playback has been stopped or paused.
     *
     * @throws IllegalStateException if the internal player engine has not been
     * initialized.
     */
    status_t stop();

    /**
     * Pauses playback. Call start() to resume.
     *
     * @throws IllegalStateException if the internal player engine has not been
     * initialized.
     */
    status_t pause();

    /**
     * playback complete.
     *
     * @throws IllegalStateException if the internal player engine has not been
     * initialized.
     */
    status_t playbackComplete();

public: 
    /**
     * Returns the width of the video.
     *
     * @return the width of the video, or 0 if there is no video,
     * no display surface was set, or the width has not been determined
     * yet. The OnVideoSizeChangedListener can be registered via
     * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
     * to provide a notification when the width is available.
     */
    status_t getVideoWidth(int *width);

    /**
     * Returns the height of the video.
     *
     * @return the height of the video, or 0 if there is no video,
     * no display surface was set, or the height has not been determined
     * yet. The OnVideoSizeChangedListener can be registered via
     * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
     * to provide a notification when the height is available.
     */
    status_t getVideoHeight(int *height);

    /**
     * Checks whether the MediaPlayer is playing.
     *
     * @return true if currently playing, false otherwise
     * @throws IllegalStateException if the internal player engine has not been
     * initialized or has been released.
     */
    bool isPlaying();

    /**
     * Seeks to specified time position.
     *
     * @param msec the offset in milliseconds from the start to seek to
     * @throws IllegalStateException if the internal player engine has not been
     * initialized
     */
    status_t seekTo(int msec);

    /**
     * Gets the current playback position.
     *
     * @return the current position in milliseconds
     */
    int getCurrentPosition();

    /**
     * Gets the duration of the file.
     *
     * @return the duration in milliseconds
     */
    int getDuration();

    /**
     * Resets the MediaPlayer to its uninitialized state. After calling
     * this method, you will have to initialize it again by setting the
     * data source and calling prepare().
     */
    status_t reset();

    /**
     * Sets the audio stream type for this MediaPlayer. See {@link AudioManager}
     * for a list of stream types. Must call this method before prepare() or
     * prepareAsync() in order for the target stream type to become effective
     * thereafter.
     *
     * @param streamtype the audio stream type, e.g. AUDIO_STREAM_MUSIC.
     * @see android.media.AudioManager
     */
    status_t setAudioStreamType(int streamtype);

    /**
     * Sets the player to be looping or non-looping.
     *
     * @param looping whether to loop or not
     */
    status_t setLooping(bool looping);

    /**
     * Checks whether the MediaPlayer is looping or non-looping.
     *
     * @return true if the MediaPlayer is currently looping, false otherwise
     */
    bool isLooping();

    /**
     * Sets the volume on this player.
     * This API is recommended for balancing the output of audio streams
     * within an application. Unless you are writing an application to
     * control user settings, this API should be used in preference to
     * {@link AudioManager#setStreamVolume(int, int, int)} which sets the volume of ALL streams of
     * a particular type. Note that the passed volume values are raw scalars.
     * UI controls should be scaled logarithmically.
     *
     * @param leftVolume left volume scalar, [0, 1.0]
     * @param rightVolume right volume scalar, [0, 1.0]
     */
    status_t setVolume(float leftVolume, float rightVolume);
    status_t getVolume(float *leftVolume, float *rightVolume);
    status_t setMuteMode(bool mute);
    status_t getMuteMode(BOOL* pMute);

    /**
     * set audio output card type.
     * 0-audiocodec; 1-hdmi_audio
     */
    status_t setAOCardType(PCM_CARD_TYPE_E cardId);

    /**
     * Sets the audio session ID.
     *
     * @param sessionId the audio session ID.
     * The audio session ID is a system wide unique identifier for the audio stream played by
     * this MediaPlayer instance.
     * The primary use of the audio session ID  is to associate audio effects to a particular
     * instance of MediaPlayer: if an audio session ID is provided when creating an audio effect,
     * this effect will be applied only to the audio content of media players within the same
     * audio session and not to the output mix.
     * When created, a MediaPlayer instance automatically generates its own audio session ID.
     * However, it is possible to force this player to be part of an already existing audio session
     * by calling this method.
     * This method must be called before one of the overloaded <code> setDataSource </code> methods.
     * @throws IllegalStateException if it is called in an invalid state
     */
    status_t setAudioSessionId(int sessionId);

    /**
     * Returns the audio session ID.
     *
     * @return the audio session ID. {@see #setAudioSessionId(int)}
     * Note that the audio session ID is 0 only if a problem occured when the MediaPlayer was contructed.
     */
    int getAudioSessionId();

    /**
     * Attaches an auxiliary effect to the player. A typical auxiliary effect is a reverberation
     * effect which can be applied on any sound source that directs a certain amount of its
     * energy to this effect. This amount is defined by setAuxEffectSendLevel().
     * {@see #setAuxEffectSendLevel(float)}.
     * <p>After creating an auxiliary effect (e.g.
     * {@link android.media.audiofx.EnvironmentalReverb}), retrieve its ID with
     * {@link android.media.audiofx.AudioEffect#getId()} and use it when calling this method
     * to attach the player to the effect.
     * <p>To detach the effect from the player, call this method with a null effect id.
     * <p>This method must be called after one of the overloaded <code> setDataSource </code>
     * methods.
     * @param effectId system wide unique id of the effect to attach
     */
    status_t attachAuxEffect(int effectId);

    /**
     * Sets the send level of the player to the attached auxiliary effect
     * {@see #attachAuxEffect(int)}. The level value range is 0 to 1.0.
     * <p>By default the send level is 0, so even if an effect is attached to the player
     * this method must be called for the effect to be applied.
     * <p>Note that the passed level value is a raw scalar. UI controls should be scaled
     * logarithmically: the gain applied by audio framework ranges from -72dB to 0dB,
     * so an appropriate conversion from linear UI input x to level is:
     * x == 0 -> level = 0
     * 0 < x <= R -> level = 10^(72*(x-R)/20/R)
     * @param level send level scalar
     */
    status_t setAuxEffectSendLevel(float level);

    /**
     * if not grant, player will only play video and others.
     */
    status_t grantPlayAudio(bool bGrant);

    /**
     * Class for MediaPlayer to return each audio/video/subtitle track's metadata.
     *
     * @see android.media.MediaPlayer#getTrackInfo
     */
    class TrackInfo {
    public:
        /**
         * Gets the track type.
         * @return TrackType which indicates if the track is video, audio, timed text.
         */
        int getTrackType() const;
        /**
         * Gets the language code of the track.
         * @return a language code in either way of ISO-639-1 or ISO-639-2.
         * When the language is unknown or could not be determined,
         * ISO-639-2 language code, "und", is returned.
         */
        const std::string& getLanguage() const;
        enum
        {
            MEDIA_TRACK_TYPE_UNKNOWN = 0,
            MEDIA_TRACK_TYPE_VIDEO = 1,
            MEDIA_TRACK_TYPE_AUDIO = 2,
            MEDIA_TRACK_TYPE_TIMEDTEXT = 3,
        };
        int mTrackType;   //MEDIA_TRACK_TYPE_VIDEO
        std::string mLanguage;

        TrackInfo(int trackType, std::string language);
        TrackInfo(const TrackInfo& trackInfo);
        TrackInfo& operator=(const TrackInfo& trackInfo);
    };

    /**
     * Returns an array of track information.
     *
     * @return Array of track info. The total number of tracks is the array length.
     * Must be called again if an external timed text source has been added after any of the
     * addTimedTextSource methods are called.
     * @throws IllegalStateException if it is called in an invalid state.
     */
	void getTrackInfo(std::vector<TrackInfo> &trackInfo);

    /* Do not change these values without updating their counterparts
     * in include/media/stagefright/MediaDefs.h and media/libstagefright/MediaDefs.cpp!
     */
    /**
     * MIME type for SubRip (SRT) container. Used in addTimedTextSource APIs.
     */
    static const char MEDIA_MIMETYPE_TEXT_SUBRIP[];    // = "application/x-subrip";

    /* Do not change these values without updating their counterparts
     * in media/CedarX-Projects/CedarX/libutil/CDX_MediaDefs.h
     * and media/CedarX-Projects/CedarX/libutil/CDX_MediaDefs.cpp
     */    
    static const char MEDIA_MIMETYPE_TEXT_IDXSUB[] ;    //= "application/idx-sub";
    static const char MEDIA_MIMETYPE_TEXT_SUB[] 	;    //= "application/sub";
    static const char MEDIA_MIMETYPE_TEXT_SMI[]	;    //= "text/smi";
    static const char MEDIA_MIMETYPE_TEXT_RT[]	    ;    //= "text/rt";
    static const char MEDIA_MIMETYPE_TEXT_TXT[] 	;    //= "text/txt";
    static const char MEDIA_MIMETYPE_TEXT_SSA[] 	;    //= "text/ssa";
    static const char MEDIA_MIMETYPE_TEXT_AQT[] 	;    //= "text/aqt";
    static const char MEDIA_MIMETYPE_TEXT_JSS[] 	;    //= "text/jss";
    static const char MEDIA_MIMETYPE_TEXT_JS[]	    ;    //= "text/js";
    static const char MEDIA_MIMETYPE_TEXT_ASS[] 	;    //= "text/ass";
    static const char MEDIA_MIMETYPE_TEXT_VSF[] 	;    //= "text/vsf";
    static const char MEDIA_MIMETYPE_TEXT_TTS[] 	;    //= "text/tts";
    static const char MEDIA_MIMETYPE_TEXT_STL[] 	;    //= "text/stl";
    static const char MEDIA_MIMETYPE_TEXT_ZEG[] 	;    //= "text/zeg";
    static const char MEDIA_MIMETYPE_TEXT_OVR[] 	;    //= "text/ovr";
    static const char MEDIA_MIMETYPE_TEXT_DKS[] 	;    //= "text/dks";
    static const char MEDIA_MIMETYPE_TEXT_LRC[] 	;    //= "text/lrc";
    static const char MEDIA_MIMETYPE_TEXT_PAN[] 	;    //= "text/pan";
    static const char MEDIA_MIMETYPE_TEXT_SBT[] 	;    //= "text/sbt";
    static const char MEDIA_MIMETYPE_TEXT_VKT[] 	;    //= "text/vkt";
    static const char MEDIA_MIMETYPE_TEXT_PJS[] 	;    //= "text/pjs";
    static const char MEDIA_MIMETYPE_TEXT_MPL[] 	;    //= "text/mpl";
    static const char MEDIA_MIMETYPE_TEXT_SCR[] 	;    //= "text/scr";
    static const char MEDIA_MIMETYPE_TEXT_PSB[] 	;    //= "text/psb";
    static const char MEDIA_MIMETYPE_TEXT_ASC[] 	;    //= "text/asc";
    static const char MEDIA_MIMETYPE_TEXT_RTF[] 	;    //= "text/rtf";
    static const char MEDIA_MIMETYPE_TEXT_S2K[] 	;    //= "text/s2k";
    static const char MEDIA_MIMETYPE_TEXT_SST[] 	;    //= "text/sst";
    static const char MEDIA_MIMETYPE_TEXT_SON[] 	;    //= "text/son";
    static const char MEDIA_MIMETYPE_TEXT_SSTS[] 	;    //= "text/ssts";

    /* TODO: Limit the total number of external timed text source to a reasonable number.
     */
    /**
     * Adds an external timed text source file.
     *
     * Currently supported format is SubRip with the file extension .srt, case insensitive.
     * Note that a single external timed text source may contain multiple tracks in it.
     * One can find the total number of available tracks using {@link #getTrackInfo()} to see what
     * additional tracks become available after this method call.
     *
     * @param path The file path of external timed text source file.
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IOException if the file cannot be accessed or is corrupted.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    void addTimedTextSource(std::string path, std::string mimeType);

    /**
     * Adds an external timed text source file (FileDescriptor).
     *
     * It is the caller's responsibility to close the file descriptor.
     * It is safe to do so as soon as this call returns.
     *
     * Currently supported format is SubRip. Note that a single external timed text source may
     * contain multiple tracks in it. One can find the total number of available tracks
     * using {@link #getTrackInfo()} to see what additional tracks become available
     * after this method call.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    void addTimedTextSource(int fd, std::string mimeType);

    /**
     * Adds an external timed text file (FileDescriptor).
     *
     * It is the caller's responsibility to close the file descriptor.
     * It is safe to do so as soon as this call returns.
     *
     * Currently supported format is SubRip. Note that a single external timed text source may
     * contain multiple tracks in it. One can find the total number of available tracks
     * using {@link #getTrackInfo()} to see what additional tracks become available
     * after this method call.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param offset the offset into the file where the data to be played starts, in bytes
     * @param length the length in bytes of the data to be played
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    void addTimedTextSource(int fd, int64_t offset, int64_t length, std::string mimeType);

    /**
     * Selects a track.
     * <p>
     * If a MediaPlayer is in invalid state, it throws an IllegalStateException exception.
     * If a MediaPlayer is in <em>Started</em> state, the selected track is presented immediately.
     * If a MediaPlayer is not in Started state, it just marks the track to be played.
     * </p>
     * <p>
     * In any valid state, if it is called multiple times on the same type of track (ie. Video,
     * Audio, Timed Text), the most recent one will be chosen.
     * </p>
     * <p>
     * The first audio and video tracks are selected by default if available, even though
     * this method is not called. However, no timed text track will be selected until
     * this function is called.
     * </p>
     * <p>
     * Currently, only timed text tracks or audio tracks can be selected via this method.
     * In addition, the support for selecting an audio track at runtime is pretty limited
     * in that an audio track can only be selected in the <em>Prepared</em> state.
     * </p>
     * @param index the index of the track to be selected. The valid range of the index
     * is 0..total number of track - 1. The total number of tracks as well as the type of
     * each individual track can be found by calling {@link #getTrackInfo()} method.
     * @throws IllegalStateException if called in an invalid state.
     *
     * @see android.media.MediaPlayer#getTrackInfo
     */
    void selectTrack(int index);

    /**
     * Deselect a track.
     * <p>
     * Currently, the track must be a timed text track and no audio or video tracks can be
     * deselected. If the timed text track identified by index has not been
     * selected before, it throws an exception.
     * </p>
     * @param index the index of the track to be deselected. The valid range of the index
     * is 0..total number of tracks - 1. The total number of tracks as well as the type of
     * each individual track can be found by calling {@link #getTrackInfo()} method.
     * @throws IllegalStateException if called in an invalid state.
     *
     * @see android.media.MediaPlayer#getTrackInfo
     */
    void deselectTrack(int index);
    
private:
    void selectOrDeselectTrack(int index, bool select);
    status_t start_l();
    status_t pause_l();
    status_t processSeekTo(int msec);
    status_t seekTo_l(int msec);
    status_t stop_l();
    status_t reset_l();
    int getCurrentPosition_l();

    /**
     * Interface definition for a callback to be invoked when the media
     * source is ready for playback.
     */
public:
    class OnPreparedListener
    {
    public:
        /**
         * Called when the media file is ready for playback.
         *
         * @param mp the MediaPlayer that is ready for playback
         */
		OnPreparedListener(){}
		virtual ~OnPreparedListener(){}
        virtual void onPrepared(EyeseePlayer *pMp) = 0;
    };

    /**
     * Register a callback to be invoked when the media source is ready
     * for playback.
     *
     * @param listener the callback that will be run
     */
    void setOnPreparedListener(OnPreparedListener *pListener);

private:
    OnPreparedListener *mOnPreparedListener;

    /**
     * Interface definition for a callback to be invoked when playback of
     * a media source has completed.
     */
public:
    class OnCompletionListener
    {
    public:
        /**
         * Called when the end of a media source is reached during playback.
         *
         * @param mp the MediaPlayer that reached the end of the file
         */
		OnCompletionListener(){}
		virtual ~OnCompletionListener(){}         
        virtual void onCompletion(EyeseePlayer *pMp)=0;
    };

    /**
     * Register a callback to be invoked when the end of a media source
     * has been reached during playback.
     *
     * @param listener the callback that will be run
     */
    void setOnCompletionListener(OnCompletionListener *pListener);
    
private:
    OnCompletionListener *mOnCompletionListener;

    /**
     * Interface definition of a callback to be invoked indicating
     * the completion of a seek operation.
     */
public:
    class OnSeekCompleteListener
    {
    public:
		OnSeekCompleteListener(){}
		virtual ~OnSeekCompleteListener(){}
        /**
         * Called to indicate the completion of a seek operation.
         *
         * @param mp the MediaPlayer that issued the seek operation
         */
        virtual void onSeekComplete(EyeseePlayer *mp) = 0;
    };

    /**
     * Register a callback to be invoked when a seek operation has been
     * completed.
     *
     * @param listener the callback that will be run
     */
    void setOnSeekCompleteListener(OnSeekCompleteListener *pListener);

private:
    OnSeekCompleteListener *mOnSeekCompleteListener;

    /**
     * Interface definition of a callback to be invoked when the
     * video size is first known or updated
     */
public:
    class OnVideoSizeChangedListener
    {
    public:
		OnVideoSizeChangedListener(){}
		virtual ~OnVideoSizeChangedListener(){}
        /**
         * Called to indicate the video size
         *
         * The video size (width and height) could be 0 if there was no video,
         * no display surface was set, or the value was not determined yet.
         *
         * @param mp        the MediaPlayer associated with this callback
         * @param width     the width of the video
         * @param height    the height of the video
         */
        virtual void onVideoSizeChanged(EyeseePlayer *pMp, int width, int height) = 0;
    };

    /**
     * Register a callback to be invoked when the video size is
     * known or updated.
     *
     * @param listener the callback that will be run
     */
    void setOnVideoSizeChangedListener(OnVideoSizeChangedListener *pListener);

private:
    OnVideoSizeChangedListener *mOnVideoSizeChangedListener;

    /**
     * Interface definition of a callback to be invoked when a
     * timed text is available for display.
     */
public:
    class OnTimedTextListener
    {
    public:
		OnTimedTextListener(){}
		virtual ~OnTimedTextListener(){}
        /**
         * Called to indicate an avaliable timed text
         *
         * @param mp             the MediaPlayer associated with this callback
         * @param text           the timed text sample which contains the text
         *                       needed to be displayed and the display format.
         */
        //virtual void onTimedText(EyeseePlayer *pMp, CedarTimedText *pText) = 0;
    };

    /**
     * Register a callback to be invoked when a timed text is available
     * for display.
     *
     * @param listener the callback that will be run
     */
    void setOnTimedTextListener(OnTimedTextListener *pListener);
    
private:
    OnTimedTextListener *mOnTimedTextListener;

public:
    /* Do not change these values without updating their counterparts
     * in include/media/mediaplayer.h!
     */
    /** Unspecified media player error.
     * @see android.media.MediaPlayer.OnErrorListener
     */
    enum
    {
        MEDIA_ERROR_UNKNOWN = 1,

        /** Media server died. In this case, the application must release the
         * MediaPlayer object and instantiate a new one.
         * @see android.media.MediaPlayer.OnErrorListener
         */
        MEDIA_ERROR_SERVER_DIED = 100,

        /** The video is streamed and its container is not valid for progressive
         * playback i.e the video's index (e.g moov atom) is not at the start of the
         * file.
         * @see android.media.MediaPlayer.OnErrorListener
         */
        MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,

    	/* add by eric_wang. start {{----------------------------------- */
    	/* 2013-05-07 15:07:00 */
    	MEDIA_ERROR_OUT_OF_MEMORY = 900,
        /* add by eric_wang. end -----------------------------------}}*/

        /** File or network related operation errors. */
        MEDIA_ERROR_IO = -1004,
        /** Bitstream is not conforming to the related coding standard or file spec. */
        MEDIA_ERROR_MALFORMED = -1007,
        /** Bitstream is conforming to the related coding standard or file spec, but
         * the media framework does not support the feature. */
        MEDIA_ERROR_UNSUPPORTED = -1010,
        /** Some operation takes too long to complete, usually more than 3-5 seconds. */
        MEDIA_ERROR_TIMED_OUT = -110,
    };
    /**
     * Interface definition of a callback to be invoked when there
     * has been an error during an asynchronous operation (other errors
     * will throw exceptions at method call time).
     */
    class OnErrorListener
    {
    public:
		OnErrorListener(){}
		virtual ~OnErrorListener(){};
        /**
         * Called to indicate an error.
         *
         * @param mp      the MediaPlayer the error pertains to
         * @param what    the type of error that has occurred:
         * <ul>
         * <li>{@link #MEDIA_ERROR_UNKNOWN}
         * <li>{@link #MEDIA_ERROR_SERVER_DIED}
         * </ul>
         * @param extra an extra code, specific to the error. Typically
         * implementation dependent.
         * <ul>
         * <li>{@link #MEDIA_ERROR_IO}
         * <li>{@link #MEDIA_ERROR_MALFORMED}
         * <li>{@link #MEDIA_ERROR_UNSUPPORTED}
         * <li>{@link #MEDIA_ERROR_TIMED_OUT}
         * </ul>
         * @return True if the method handled the error, false if it didn't.
         * Returning false, or not having an OnErrorListener at all, will
         * cause the OnCompletionListener to be called.
         */
        virtual bool onError(EyeseePlayer *pMp, int what, int extra) = 0;
    };

    /**
     * Register a callback to be invoked when an error has happened
     * during an asynchronous operation.
     *
     * @param listener the callback that will be run
     */
    void setOnErrorListener(OnErrorListener *pListener);
    
private:
    OnErrorListener *mOnErrorListener;

public:
    /* Do not change these values without updating their counterparts
     * in include/media/mediaplayer.h!
     */
    /** Unspecified media player info.
     * @see android.media.MediaPlayer.OnInfoListener
     */
    enum
    {
        MEDIA_INFO_UNKNOWN = 1,

        /** The player was started because it was used as the next player for another
         * player, which just completed playback.
         * @see android.media.MediaPlayer.OnInfoListener
         * @hide
         */
        MEDIA_INFO_STARTED_AS_NEXT = 2,

        /** The player just pushed the very first video frame for rendering.
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_VIDEO_RENDERING_START = 3,

        /** The video is too complex for the decoder: it can't decode frames fast
         *  enough. Possibly only the audio plays fine at this stage.
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_VIDEO_TRACK_LAGGING = 700,

        /** MediaPlayer is temporarily pausing playback internally in order to
         * buffer more data.
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_BUFFERING_START = 701,

        /** MediaPlayer is resuming playback after filling buffers.
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_BUFFERING_END = 702,

        /** Bad interleaving means that a media has been improperly interleaved or
         * not interleaved at all, e.g has all the video samples first then all the
         * audio ones. Video is playing but a lot of disk seeks may be happening.
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_BAD_INTERLEAVING = 800,

        /** The media cannot be seeked (e.g live stream)
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_NOT_SEEKABLE = 801,

        /** A new set of metadata is available.
         * @see android.media.MediaPlayer.OnInfoListener
         */
        MEDIA_INFO_METADATA_UPDATE = 802,

        /** Failed to handle timed text track properly.
         * @see android.media.MediaPlayer.OnInfoListener
         *
         * {@hide}
         */
        MEDIA_INFO_TIMED_TEXT_ERROR = 900,
    };

    /**
     * Interface definition of a callback to be invoked to communicate some
     * info and/or warning about the media or its playback.
     */
    class OnInfoListener
    {
    public:
        /**
         * Called to indicate an info or a warning.
         *
         * @param mp      the MediaPlayer the info pertains to.
         * @param what    the type of info or warning.
         * <ul>
         * <li>{@link #MEDIA_INFO_UNKNOWN}
         * <li>{@link #MEDIA_INFO_VIDEO_TRACK_LAGGING}
         * <li>{@link #MEDIA_INFO_VIDEO_RENDERING_START}
         * <li>{@link #MEDIA_INFO_BUFFERING_START}
         * <li>{@link #MEDIA_INFO_BUFFERING_END}
         * <li>{@link #MEDIA_INFO_BAD_INTERLEAVING}
         * <li>{@link #MEDIA_INFO_NOT_SEEKABLE}
         * <li>{@link #MEDIA_INFO_METADATA_UPDATE}
         * </ul>
         * @param extra an extra code, specific to the info. Typically
         * implementation dependent.
         * @return True if the method handled the info, false if it didn't.
         * Returning false, or not having an OnErrorListener at all, will
         * cause the info to be discarded.
         */
		OnInfoListener(){}
		virtual ~OnInfoListener(){}     
        virtual bool onInfo(EyeseePlayer *pMp, int what, int extra) = 0;
    };

    /**
     * Register a callback to be invoked when an info/warning is available.
     *
     * @param listener the callback that will be run
     */
    void setOnInfoListener(OnInfoListener *pListener);

private:
    OnInfoListener *mOnInfoListener;

    /* add by Gary. start {{----------------------------------- */
    /* 2011-9-13 9:50:50 */
    /* expend interfaces about subtitle, track and so on */
public:
    /**
     * Set the subtitle¡¯s charset. If the underlying mediaplayer can absolutely parse the charset 
     * of the subtitles, still use the parsed charset; otherwise, use the charset argument.
     * <p>
     * 
     * @param charset  the canonical name of a charset.
     * @return ==0 means successful, !=0 means failed.
     */
    status_t setSubCharset(std::string charset);
    
    /**
    * Get the subtitle¡¯s charset.
    * <p>
    * 
    * @return the canonical name of a charset.
    */
    status_t getSubCharset(std::string &charset); 
    
    /**
     * Set the subtitle¡¯s delay time.
     * <p>
     * 
     * @param time delay time in milliseconds. It can be <0.
     * @return ==0 means successful, !=0 means failed.
     */
    status_t setSubDelay(int time); 
    
    /**
     * Get the subtitle¡¯s delay time.
     * <p>
     * 
     * @return delay time in milliseconds.
     */
    int getSubDelay();


    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-14 */
    /* support scale mode */
    /**
     * enable or disable scale mode for playing video.
     * <p>
     * 
	 * @param enable if true, enable the scale mode, else disable the scale mode.
	 * @param width  the expected width of the video. Only valid when enable.
	 * @param height  the expected height of the video. Only valid when enable.
     */
    status_t enableScaleMode(bool enable, int width, int height);
    /* add by Gary. end   -----------------------------------}} */

    /* clockwise rotation: val=0 no rotation, val=1 90 degree; val=2 180 degree, val=3 270 degree */
    status_t setRotation(ROTATE_E val);

private:
    /* Do not change these values without updating their counterparts
     * in include/media/mediaplayer.h!
     */
    enum
    {
        MEDIA_NOP = 0, // interface test message
        MEDIA_PREPARED = 1,
        MEDIA_PLAYBACK_COMPLETE = 2,
        MEDIA_BUFFERING_UPDATE = 3,
        MEDIA_SEEK_COMPLETE = 4,
        MEDIA_SET_VIDEO_SIZE = 5,
        MEDIA_TIMED_TEXT = 99,
        MEDIA_ERROR = 100,
        MEDIA_INFO = 200,
    	MEDIA_SOURCE_DETECTED = 234,	//Add by Bevis.

        MEDIA_NOTIFY_EOF = 0x100,   //arg1:mod_id MOD_ID_DEMUX
    };
	class EventHandler : public CallbackDispatcher {
	private:
		EyeseePlayer *mPlayer;
	public:
		EventHandler(EyeseePlayer *player):
			mPlayer(player) {
		}
		~EventHandler() {
		}
		virtual void handleMessage(const CallbackMessage &msg);
	};
    EventHandler *mEventHandler;
    static void postEventFromNative(EyeseePlayer *pPlayer, int what, int arg1, int arg2, const std::shared_ptr<CMediaMemory>* pDataPtr);
    void notify(MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData);
    static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData);
	inline bool isVideoScalingModeSupported(int mode) {
		return (mode == NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW ||
			mode == NATIVE_WINDOW_SCALING_MODE_SCALE_CROP );
	}

private:
	static bool availableMimeTypeForExternalSource(std::string mimeType);
    status_t config_VDEC_CHN_ATTR_S(DEMUX_VIDEO_STREAM_INFO_S *pVideoStreamInfo);
    status_t config_ADEC_CHN_ATTR_S(DEMUX_AUDIO_STREAM_INFO_S *pAudioStreamInfo);
    status_t config_AIO_ATTR_S(DEMUX_AUDIO_STREAM_INFO_S *pStreamInfo);
    static void* EyeseeCommandThread(void *pThreadData);

    Mutex mLock;
    media_player_states mCurrentState;
    bool mbGrantAudio;
    bool mbSeekStart;
    int mSeekStartPosition;    //unit:ms
    bool mLoop; //loop play.
    int mVideoScalingMode;  //VIDEO_SCALING_MODE_SCALE_TO_FIT, NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW
    int mCurSubtitleTrackIndex; //-1:not select.
    int mTempPosition;  //unit:ms
    int mMaxVdecOutputWidth;
    int mMaxVdecOutputHeight;
    int mDisplayWidth;   //display width
    int mDisplayHeight;
    PCM_CARD_TYPE_E mAOCardId;
    ROTATE_E mInitRotation;  //clockwise rotation: val=0 no rotation, val=1 90 degree; val=2 180 degree, val=3 270 degree
    PIXEL_FORMAT_E mUserSetPixelFormat; //store user set output pixel format. ref to setOutputPixelFormat().
    pthread_t mCommandThreadId;
    Mutex mNotifyEofLock;
    bool mDmxNotifyEof;
    bool mVdecNotifyEof;
    bool mAdecNotifyEof;
    bool mVONotifyEof;
    bool mAONotifyEof;
    EyeseeMessageQueue mCommandQueue;
    enum PlayerCommandType
    {
        PLAYER_COMMAND_SEEK = 0x100,
        PLAYER_COMMAND_STOP,
    };

    unsigned int mVdecInputBufferSize;

    //MPP components
    DEMUX_CHN mDmxChn;
    DEMUX_CHN_ATTR_S mDmxChnAttr;
    VDEC_CHN mVdecChn;
    VDEC_CHN_ATTR_S mVdecAttr;
    ADEC_CHN mAdecChn;
    ADEC_CHN_ATTR_S mAdecAttr;
    VO_LAYER mVOLayer;
    VO_CHN mVOChn;
    VO_CHN_ATTR_S mVOAttr;
    AUDIO_DEV mAODev;
    AO_CHN mAOChn;
    AIO_ATTR_S mAioAttr;
    CLOCK_CHN mClockChn;
    CLOCK_CHN_ATTR_S mClockChnAttr;
};

};

#endif  //__IPCLINUX_EYESEEPLAYER_H__

