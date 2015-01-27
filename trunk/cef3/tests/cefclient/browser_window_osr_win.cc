// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser_window_osr_win.h"

#include "cefclient/main_message_loop.h"

namespace client {

BrowserWindowOsrWin::BrowserWindowOsrWin(BrowserWindowWin::Delegate* delegate,
                                         const std::string& startup_url,
                                         bool transparent,
                                         bool show_update_rect)
    : BrowserWindowWin(delegate),
      transparent_(transparent),
      osr_hwnd_(NULL) {
  osr_window_ = new OsrWindowWin(this, transparent, show_update_rect);
  client_handler_ = new ClientHandlerOsr(this, osr_window_.get(), startup_url);
}

void BrowserWindowOsrWin::CreateBrowser(HWND parent_hwnd,
                                        const RECT& rect,
                                        const CefBrowserSettings& settings) {
  REQUIRE_MAIN_THREAD();

  // Create the new browser and native window on the UI thread.
  osr_window_->CreateBrowser(parent_hwnd, rect, client_handler_, settings,
                             client_handler_->startup_url());
}

void BrowserWindowOsrWin::GetPopupConfig(HWND temp_hwnd,
                                         CefWindowInfo& windowInfo,
                                         CefRefPtr<CefClient>& client,
                                         CefBrowserSettings& settings) {
  // Note: This method may be called on any thread.
  windowInfo.SetAsWindowless(temp_hwnd, transparent_);
  client = client_handler_;
}

void BrowserWindowOsrWin::ShowPopup(HWND parent_hwnd,
                                    int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();
  if (osr_window_)
    osr_window_->ShowPopup(parent_hwnd, x, y, width, height);
}

void BrowserWindowOsrWin::Show() {
  REQUIRE_MAIN_THREAD();
  if (osr_window_)
    osr_window_->Show();
}

void BrowserWindowOsrWin::Hide() {
  REQUIRE_MAIN_THREAD();
  if (osr_window_)
    osr_window_->Hide();
}

void BrowserWindowOsrWin::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();
  if (osr_window_)
    osr_window_->SetBounds(x, y, width, height);
}

void BrowserWindowOsrWin::SetFocus() {
  REQUIRE_MAIN_THREAD();
  if (osr_window_)
    osr_window_->SetFocus();
}

HWND BrowserWindowOsrWin::GetHWND() const {
  REQUIRE_MAIN_THREAD();
  return osr_hwnd_;
}

void BrowserWindowOsrWin::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();

  // Release the OSR window reference. It will be deleted on the UI thread.
  osr_window_ = NULL;

  BrowserWindowWin::OnBrowserClosed(browser);
}

void BrowserWindowOsrWin::OnOsrNativeWindowCreated(HWND hwnd) {
  REQUIRE_MAIN_THREAD();
  DCHECK(!osr_hwnd_);
  osr_hwnd_ = hwnd;
}

}  // namespace client
