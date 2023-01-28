// Copyright(c) 2023, Intel Corporation
//
// Redistribution  and  use  in source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of  source code  must retain the  above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name  of Intel Corporation  nor the names of its contributors
//   may be used to  endorse or promote  products derived  from this  software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
// IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
// LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
// CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
// SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
// INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
// CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif  // HAVE_CONFIG_H

#include <opae/types.h>

#include "grpc_client.hpp"
#include "mock/opae_std.h"
#include "remote.h"

struct _remote_handle *opae_create_remote_handle(struct _remote_token *token,
                                                 fpga_handle_header *hdr) {
  struct _remote_handle *h =
      (struct _remote_handle *)opae_calloc(1, sizeof(*h));
  if (h) {
    h->hdr = *hdr;
    h->token = token;
  }
  return h;
}

void opae_destroy_remote_handle(struct _remote_handle *h) { opae_free(h); }

fpga_result __REMOTE_API__ remote_fpgaOpen(fpga_token token,
                                           fpga_handle *handle, int flags) {
  struct _remote_token *tok;
  OPAEClient *client;
  fpga_handle_header hdr;
  struct _remote_handle *h;
  fpga_result res;

  if (!token) {
    OPAE_ERR("NULL token");
    return FPGA_INVALID_PARAM;
  }

  if (!handle) {
    OPAE_ERR("NULL handle pointer");
    return FPGA_INVALID_PARAM;
  }

  tok = reinterpret_cast<_remote_token *>(token);
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  res = client->fpgaOpen(tok->hdr.token_id, flags, hdr);
  if (res == FPGA_OK) {
    h = opae_create_remote_handle(tok, &hdr);
    if (!h) {
      OPAE_ERR("calloc failed");
      return FPGA_NO_MEMORY;
    }

    *handle = h;
  }

  return res;
}
