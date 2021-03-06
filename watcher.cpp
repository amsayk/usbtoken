
#include <string>

#include "inc/token.h"
#include "inc/ep_pkcs15.h"

#include "inc/watcher.h"
#include "epToken/client_handler.h"

#include "include/cef_v8.h"

#include "inc/ep_thread_ctx.h"

#if defined(WIN32)
  #include <windows.h>
  #include <process.h>
#endif

namespace epsilon {

CefRefPtr<CefProcessMessage> Msg(const std::string &kMessageName, const char *reader = NULL) {
  CefRefPtr<CefProcessMessage> response = CefProcessMessage::Create(kMessageName);

  if(reader) {
    CefRefPtr<CefDictionaryValue> args = CefDictionaryValue::Create();
    args->SetString("reader", reader);

    response->GetArgumentList()->SetDictionary(0, args);
  }

  return response;
}

// Bring in platform-specific definitions.
#if defined(WIN32)

void WatchToken(void *lpParam) {
  volatile bool out_ = true;
  volatile DWORD sleep_ = 1000;
  sc_context *ctx = NULL;
  
  ClientHandler *handler = static_cast<ClientHandler *>(lpParam);
  ASSERT(handler != NULL);

  while(handler->app_running_) {

    if(TokenContext::isTokenPresent(&ctx)) {
		ASSERT(ctx != NULL);

      if(out_) {

        unsigned int evt;
        sc_reader *found = NULL;    

        if(sc_wait_for_event(ctx, SC_EVENT_READER_ATTACHED, &found, &evt, 0, NULL) == 0) {

          sc_ctx_detect_readers(ctx);

          /* Waiting for a card to be inserted */
          if(sc_wait_for_event(ctx, SC_EVENT_CARD_INSERTED, &found, &evt, 0, NULL) == 0) {
            
            /* Card was inserted */           
            if(evt & SC_EVENT_CARD_INSERTED) {
              CefRefPtr<CefBrowser> browser = handler->GetBrowser();            
                  ASSERT(browser.get());

              browser->SendProcessMessage(PID_RENDERER, Msg(E_TOKEN_INSERTED, found->name));

              sleep_ = 2500;
              out_ = false;
            }
          }
        }
      
      }     

	  sc_release_context(ctx);
	  ctx = NULL;
    
    } else {

      if(out_ == false) {

        /* Card was removed */
        CefRefPtr<CefBrowser> browser = handler->GetBrowser();            
            ASSERT(browser.get());

        browser->SendProcessMessage(PID_RENDERER, Msg(E_TOKEN_REMOVED));

        sleep_ = 1000;
        out_ = true;
      }
    }
      
    sleep : Sleep(sleep_); 
  }

  _endthread();

  if(handler->m_watcherHnd) {
    //CloseHandle(handler->m_watcherHnd);
    handler->m_watcherHnd = NULL;
  }

  //return 0; 
}
  
void TokenWatcher::Init(ClientHandler *handler) {
  handler->m_watcherHnd = (void *)_beginthread(WatchToken, 0, (void *)handler);
}

#endif

}