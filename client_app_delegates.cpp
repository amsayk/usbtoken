// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "client_app.h"
#include "client_renderer.h"
#include "dom_test.h"
#include "scheme_test.h"

// static
void ClientApp::CreateRenderDelegates(RenderDelegateSet& delegates) {
  client_renderer::CreateRenderDelegates(delegates);
  dom_test::CreateRenderDelegates(delegates);
}

// static
void ClientApp::RegisterCustomSchemes(
    CefRefPtr<CefSchemeRegistrar> registrar,
    std::vector<CefString>& cookiable_schemes) {
  scheme_test::RegisterCustomSchemes(registrar, cookiable_schemes);
}
