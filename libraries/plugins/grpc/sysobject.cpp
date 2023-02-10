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

#include <opae/log.h>
#include <opae/types.h>

#include "grpc_client.hpp"
#include "mock/opae_std.h"
#include "remote.h"

struct _remote_sysobject *opae_create_remote_sysobject(
    struct _remote_token *token, fpga_remote_id *rid) {
  struct _remote_sysobject *s =
      (struct _remote_sysobject *)opae_calloc(1, sizeof(*s));
  if (s) {
    s->token = token;
    s->object_id = *rid;
  }
  return s;
}

void opae_destroy_remote_sysobject(struct _remote_sysobject *s) {
  opae_free(s);
}

fpga_result __REMOTE_API__ remote_fpgaTokenGetObject(fpga_token token,
                                                     const char *name,
                                                     fpga_object *object,
                                                     int flags) {
#if 1
  _remote_token *tok;
  OPAEClient *client;
  fpga_remote_id object_id;
  fpga_result res;

  if (!token) {
    OPAE_ERR("NULL token");
    return FPGA_INVALID_PARAM;
  }

  if (!name) {
    OPAE_ERR("NULL name");
    return FPGA_INVALID_PARAM;
  }

  if (!object) {
    OPAE_ERR("NULL object pointer");
    return FPGA_INVALID_PARAM;
  }

  tok = reinterpret_cast<_remote_token *>(token);
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  res = client->fpgaTokenGetObject(tok->hdr.token_id, name, flags, object_id);
  if (res == FPGA_OK) {
    _remote_sysobject *o = opae_create_remote_sysobject(tok, &object_id);

    if (!o) {
      OPAE_ERR("calloc failed");
      *object = NULL;
      return FPGA_NO_MEMORY;
    }

    *object = o;
  }

  return res;
#else
  opae_fpgaTokenGetObject_request req;
  opae_fpgaTokenGetObject_response resp;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;
  size_t len;

  if (!token) {
    OPAE_ERR("NULL token");
    return FPGA_INVALID_PARAM;
  }

  if (!name) {
    OPAE_ERR("NULL name");
    return FPGA_INVALID_PARAM;
  }

  if (!object) {
    OPAE_ERR("NULL object pointer");
    return FPGA_INVALID_PARAM;
  }

  tok = (struct _remote_token *)token;
  req.token_id = tok->hdr.token_id;

  memset(req.name, 0, sizeof(req.name));
  len = strnlen(name, OPAE_SYSOBJECT_NAME_MAX);
  memcpy(req.name, name, len + 1);

  req.flags = flags;

  req_json = opae_encode_fpgaTokenGetObject_request_23(
      &req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaTokenGetObject_response_23(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    struct _remote_sysobject *o =
        opae_create_remote_sysobject(tok, &resp.object_id);

    if (!o) {
      OPAE_ERR("calloc failed");
      *object = NULL;
      return FPGA_NO_MEMORY;
    }

    *object = o;
  }

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaHandleGetObject(fpga_handle handle,
                                                      const char *name,
                                                      fpga_object *object,
                                                      int flags) {
#if 1
  (void)handle;
  (void)name;
  (void)object;
  (void)flags;

  return FPGA_OK;
#else
  opae_fpgaHandleGetObject_request req;
  opae_fpgaHandleGetObject_response resp;
  struct _remote_handle *h;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;
  size_t len;

  if (!handle) {
    OPAE_ERR("NULL handle");
    return FPGA_INVALID_PARAM;
  }

  if (!name) {
    OPAE_ERR("NULL name");
    return FPGA_INVALID_PARAM;
  }

  if (!object) {
    OPAE_ERR("NULL object pointer");
    return FPGA_INVALID_PARAM;
  }

  h = (struct _remote_handle *)handle;
  tok = h->token;

  req.handle_id = h->hdr.handle_id;

  memset(req.name, 0, sizeof(req.name));
  len = strnlen(name, OPAE_SYSOBJECT_NAME_MAX);
  memcpy(req.name, name, len + 1);

  req.flags = flags;

  req_json = opae_encode_fpgaHandleGetObject_request_31(
      &req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaHandleGetObject_response_31(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    struct _remote_sysobject *o =
        opae_create_remote_sysobject(tok, &resp.object_id);

    if (!o) {
      OPAE_ERR("calloc failed");
      *object = NULL;
      return FPGA_NO_MEMORY;
    }

    *object = o;
  }

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectGetObject(fpga_object parent,
                                                      const char *name,
                                                      fpga_object *object,
                                                      int flags) {
#if 1
  (void)parent;
  (void)name;
  (void)object;
  (void)flags;

  return FPGA_OK;
#else
  opae_fpgaObjectGetObject_request req;
  opae_fpgaObjectGetObject_response resp;
  struct _remote_sysobject *par;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;
  size_t len;

  if (!parent) {
    OPAE_ERR("NULL parent");
    return FPGA_INVALID_PARAM;
  }

  if (!name) {
    OPAE_ERR("NULL name");
    return FPGA_INVALID_PARAM;
  }

  if (!object) {
    OPAE_ERR("NULL object pointer");
    return FPGA_INVALID_PARAM;
  }

  par = (struct _remote_sysobject *)parent;
  tok = par->token;

  req.object_id = par->object_id;

  memset(req.name, 0, sizeof(req.name));
  len = strnlen(name, OPAE_SYSOBJECT_NAME_MAX);
  memcpy(req.name, name, len + 1);

  req.flags = flags;

  req_json = opae_encode_fpgaObjectGetObject_request_32(
      &req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectGetObject_response_32(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    struct _remote_sysobject *o =
        opae_create_remote_sysobject(tok, &resp.object_id);

    if (!o) {
      OPAE_ERR("calloc failed");
      *object = NULL;
      return FPGA_NO_MEMORY;
    }

    *object = o;
  }

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectGetObjectAt(fpga_object parent,
                                                        size_t idx,
                                                        fpga_object *object) {
#if 1
  (void)parent;
  (void)idx;
  (void)object;

  return FPGA_OK;
#else
  opae_fpgaObjectGetObjectAt_request req;
  opae_fpgaObjectGetObjectAt_response resp;
  struct _remote_sysobject *par;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;

  if (!parent) {
    OPAE_ERR("NULL parent");
    return FPGA_INVALID_PARAM;
  }

  if (!object) {
    OPAE_ERR("NULL object pointer");
    return FPGA_INVALID_PARAM;
  }

  par = (struct _remote_sysobject *)parent;
  tok = par->token;

  req.object_id = par->object_id;
  req.idx = idx;

  req_json = opae_encode_fpgaObjectGetObjectAt_request_33(
      &req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectGetObjectAt_response_33(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    struct _remote_sysobject *o =
        opae_create_remote_sysobject(tok, &resp.object_id);

    if (!o) {
      OPAE_ERR("calloc failed");
      *object = NULL;
      return FPGA_NO_MEMORY;
    }

    *object = o;
  }

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaDestroyObject(fpga_object *obj) {
#if 1
  _remote_sysobject *o;
  _remote_token *tok;
  OPAEClient *client;
  fpga_result res;

  if (!obj || !*obj) {
    OPAE_ERR("invalid sysobject pointer");
    return FPGA_INVALID_PARAM;
  }

  o = reinterpret_cast<_remote_sysobject *>(*obj);
  tok = o->token;
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  res = client->fpgaDestroyObject(o->object_id);

  if (res == FPGA_OK) {
    opae_destroy_remote_sysobject(o);
    *obj = NULL;
  }

  return res;

#else
  opae_fpgaDestroyObject_request req;
  opae_fpgaDestroyObject_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;

  if (!obj || !*obj) {
    OPAE_ERR("invalid sysobject pointer");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)*obj;
  tok = o->token;

  req.object_id = o->object_id;

  req_json =
      opae_encode_fpgaDestroyObject_request_24(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaDestroyObject_response_24(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    opae_destroy_remote_sysobject(o);
    *obj = NULL;
  }

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectGetSize(fpga_object obj,
                                                    uint32_t *size, int flags) {
#if 1
  _remote_sysobject *o;
  _remote_token *tok;
  OPAEClient *client;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!size) {
    OPAE_ERR("NULL size pointer");
    return FPGA_INVALID_PARAM;
  }

  o = reinterpret_cast<_remote_sysobject *>(obj);
  tok = o->token;
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  return client->fpgaObjectGetSize(o->object_id, flags, *size);

#else
  opae_fpgaObjectGetSize_request req;
  opae_fpgaObjectGetSize_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!size) {
    OPAE_ERR("NULL size pointer");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)obj;
  tok = o->token;

  req.object_id = o->object_id;
  req.flags = flags;

  req_json =
      opae_encode_fpgaObjectGetSize_request_27(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectGetSize_response_27(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) *size = resp.value;

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectRead64(fpga_object obj,
                                                   uint64_t *value, int flags) {
#if 1
  _remote_sysobject *o;
  _remote_token *tok;
  OPAEClient *client;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!value) {
    OPAE_ERR("NULL value pointer");
    return FPGA_INVALID_PARAM;
  }

  o = reinterpret_cast<_remote_sysobject *>(obj);
  tok = o->token;
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  return client->fpgaObjectRead64(o->object_id, flags, *value);

#else
  opae_fpgaObjectRead64_request req;
  opae_fpgaObjectRead64_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!value) {
    OPAE_ERR("NULL value pointer");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)obj;
  tok = o->token;

  req.object_id = o->object_id;
  req.flags = flags;

  req_json =
      opae_encode_fpgaObjectRead64_request_29(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectRead64_response_29(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) *value = resp.value;

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectRead(fpga_object obj,
                                                 uint8_t *buffer, size_t offset,
                                                 size_t blen, int flags) {
#if 1
  _remote_sysobject *o;
  _remote_token *tok;
  OPAEClient *client;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!buffer) {
    OPAE_ERR("NULL buffer");
    return FPGA_INVALID_PARAM;
  }

  if (!blen) {
    OPAE_ERR("Invalid buffer length");
    return FPGA_INVALID_PARAM;
  }

  o = reinterpret_cast<_remote_sysobject *>(obj);
  tok = o->token;
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  return client->fpgaObjectRead(o->object_id, buffer, offset, blen, flags);

#else
  opae_fpgaObjectRead_request req;
  opae_fpgaObjectRead_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;
  size_t len;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!buffer) {
    OPAE_ERR("NULL buffer");
    return FPGA_INVALID_PARAM;
  }

  if (!blen) {
    OPAE_ERR("Invalid buffer length");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)obj;
  tok = o->token;

  req.object_id = o->object_id;
  req.offset = offset;
  req.len = blen - 1;
  req.flags = flags;

  req_json =
      opae_encode_fpgaObjectRead_request_28(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectRead_response_28(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    len = strnlen(resp.value, blen - 1);
    memcpy(buffer, resp.value, len + 1);
  }

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectWrite64(fpga_object obj,
                                                    uint64_t value, int flags) {
#if 1
  (void)obj;
  (void)value;
  (void)flags;

  return FPGA_OK;
#else
  opae_fpgaObjectWrite64_request req;
  opae_fpgaObjectWrite64_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)obj;
  tok = o->token;

  req.object_id = o->object_id;
  req.value = value;
  req.flags = flags;

  req_json =
      opae_encode_fpgaObjectWrite64_request_30(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectWrite64_response_30(resp_json, &resp))
    return FPGA_EXCEPTION;

  return resp.result;
#endif
}

fpga_result __REMOTE_API__
remote_fpgaObjectGetType(fpga_object obj, enum fpga_sysobject_type *type) {
#if 1
  _remote_sysobject *o;
  _remote_token *tok;
  OPAEClient *client;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!type) {
    OPAE_ERR("NULL type pointer");
    return FPGA_INVALID_PARAM;
  }

  o = reinterpret_cast<_remote_sysobject *>(obj);
  tok = o->token;
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  return client->fpgaObjectGetType(o->object_id, *type);

#else
  opae_fpgaObjectGetType_request req;
  opae_fpgaObjectGetType_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!type) {
    OPAE_ERR("NULL type pointer");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)obj;
  tok = o->token;

  req.object_id = o->object_id;

  req_json =
      opae_encode_fpgaObjectGetType_request_25(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectGetType_response_25(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) *type = resp.type;

  return resp.result;
#endif
}

fpga_result __REMOTE_API__ remote_fpgaObjectGetName(fpga_object obj, char *name,
                                                    size_t max_len) {
#if 1
  _remote_sysobject *o;
  _remote_token *tok;
  OPAEClient *client;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!name) {
    OPAE_ERR("NULL name");
    return FPGA_INVALID_PARAM;
  }

  o = reinterpret_cast<_remote_sysobject *>(obj);
  tok = o->token;
  client = reinterpret_cast<OPAEClient *>(tok->comms->client);

  return client->fpgaObjectGetName(o->object_id, name, max_len);

#else
  opae_fpgaObjectGetName_request req;
  opae_fpgaObjectGetName_response resp;
  struct _remote_sysobject *o;
  struct _remote_token *tok;
  char *req_json;
  char *resp_json = NULL;
  fpga_result res;
  size_t len;

  if (!obj) {
    OPAE_ERR("NULL obj");
    return FPGA_INVALID_PARAM;
  }

  if (!name) {
    OPAE_ERR("NULL name");
    return FPGA_INVALID_PARAM;
  }

  o = (struct _remote_sysobject *)obj;
  tok = o->token;

  req.object_id = o->object_id;

  req_json =
      opae_encode_fpgaObjectGetName_request_26(&req, tok->json_to_string_flags);

  res = opae_client_send_and_receive(tok, req_json, &resp_json);
  if (res) return res;

  if (!opae_decode_fpgaObjectGetName_response_26(resp_json, &resp))
    return FPGA_EXCEPTION;

  if (resp.result == FPGA_OK) {
    len = strnlen(resp.name, max_len - 1);
    memcpy(name, resp.name, len + 1);
  }

  return resp.result;
#endif
}
