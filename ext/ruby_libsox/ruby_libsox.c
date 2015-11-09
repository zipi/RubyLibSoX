#include <ruby.h>
#include <sox.h>

static VALUE LibSoX;
static VALUE LibSoXFormat;
static VALUE LibSoXEffectsChain;
static VALUE LibSoXSignal;
static VALUE LibSoXEncoding;
static VALUE LibSoXEffect;
static VALUE LibSoXFormatHandler;
// static VALUE LibSoXEffectHandler;

static VALUE library_instance;

// LibSoXEffectHandler
//
//
/*
static sox_effect_handler_t const * empty_handler(void)

  static sox_effect_handler_t *handler = lsx_malloc(sizeof(sox_effect_handler_t));
  // name usage flags getopts start flow drain stop kill priv_size
  return &handler;
}

VALUE libsox_effect_handler_new(VALUE class) {
  sox_effect_handler_t const *c_effect_handler = empty_handler();
  value 
  c_effect_handler->name = 
  c_effect_handler->flags = 
  c_effect_handler->flow = 



  effect = Data_Wrap_Struct(LibSoXEffect, 0, free, c_effect);
}
*/

// LibSoXEffect
VALUE libsox_effect_new(VALUE class, VALUE name) {
  sox_effect_handler_t const *c_effect_handler;
  sox_effect_t *c_effect;
  VALUE effect;

  c_effect_handler = sox_find_effect(StringValuePtr(name));

  if(c_effect_handler == NULL) {
    return (VALUE) NULL;
  }
  c_effect = sox_create_effect(c_effect_handler);
  effect = Data_Wrap_Struct(LibSoXEffect, 0, free, c_effect);
  rb_obj_call_init(effect, 0, 0);
  return effect;
}

VALUE libsox_effect_options(VALUE effect, VALUE *args) {
  VALUE tmp;
  sox_effect_t *c_effect;
  sox_format_t *c_format;
  char *c_options[12];
  int i;

  for (i = 0; i < RARRAY_LEN(args); i++) {
    tmp = RARRAY_PTR(args)[i];
    if(RB_TYPE_P(tmp, T_STRING)) {
      c_options[i] = StringValuePtr(tmp);
    }else{
      Data_Get_Struct(tmp, sox_format_t, c_format);
      c_options[i] = (char *)c_format;
    }
  }
  Data_Get_Struct(effect, sox_effect_t, c_effect);
  return INT2NUM(sox_effect_options(c_effect, i, c_options));
}

// LibSoXEffectsChain
static void libsox_effects_chain_free(void *ptr) {
  sox_delete_effects_chain(ptr);
}

VALUE libsox_effects_chain_new(VALUE class, VALUE input, VALUE output) {
  sox_format_t *c_input, *c_output;
  sox_effects_chain_t *c_chain;
  VALUE chain;

  Data_Get_Struct(input,  sox_format_t, c_input);
  Data_Get_Struct(output, sox_format_t, c_output);
  c_chain = sox_create_effects_chain(&c_input->encoding, &c_output->encoding);
  chain = Data_Wrap_Struct(LibSoXEffectsChain, 0, libsox_effects_chain_free, c_chain);
  rb_obj_call_init(chain, 0, 0);
  return chain;
}

VALUE libsox_effects_chain_add_effect(VALUE chain, VALUE effect, VALUE in_signal, VALUE out_signal) {
  sox_effects_chain_t *c_chain;
  sox_effect_t *c_effect;
  sox_signalinfo_t *c_in_signal;
  sox_signalinfo_t *c_out_signal;

  Data_Get_Struct(in_signal, sox_signalinfo_t, c_in_signal);
  Data_Get_Struct(out_signal, sox_signalinfo_t, c_out_signal);

  Data_Get_Struct(chain, sox_effects_chain_t, c_chain);
  Data_Get_Struct(effect, sox_effect_t, c_effect);
  return INT2NUM(sox_add_effect(c_chain, c_effect, c_in_signal, c_out_signal));
}

VALUE libsox_effects_chain_flow_effects(VALUE chain) {
  sox_effects_chain_t *c_chain;

  Data_Get_Struct(chain, sox_effects_chain_t, c_chain);
  return INT2NUM(sox_flow_effects(c_chain, NULL, NULL));
}

// LibSoXSignal
VALUE libsox_signal_rate(VALUE self) {
  sox_signalinfo_t *c;

  Data_Get_Struct(self, sox_signalinfo_t, c);
  return DBL2NUM(c->rate);
}

VALUE libsox_signal_rate_set(VALUE self, VALUE rate) {
  sox_signalinfo_t *c;
  double val = NUM2DBL(rate);

  Data_Get_Struct(self, sox_signalinfo_t, c);
  c->rate = val;
  return rate;
}

VALUE libsox_signal_length(VALUE signal, VALUE bits) {
  sox_signalinfo_t *c_signal;

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  return UINT2NUM(c_signal->length);
}

VALUE libsox_signal_length_set(VALUE signal, VALUE length) {
  sox_signalinfo_t *c_signal;
  size_t val = (size_t)NUM2UINT(length);

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  c_signal->length = val;
  return length;
}

VALUE libsox_signal_precision(VALUE signal) {
  sox_signalinfo_t *c_signal;

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  return UINT2NUM(c_signal->precision);
}

VALUE libsox_signal_precision_set(VALUE signal, VALUE precision) {
  sox_signalinfo_t *c_signal;
  unsigned int val = NUM2UINT(precision);

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  c_signal->precision = val;
  return precision;
}

VALUE libsox_signal_channels(VALUE signal) {
  sox_signalinfo_t *c_signal;

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  return UINT2NUM(c_signal->channels);
}

VALUE libsox_signal_channels_set(VALUE signal, VALUE channels) {
  sox_signalinfo_t *c_signal;
  unsigned int val = NUM2UINT(channels);

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  c_signal->channels = val;
  return channels;
}

VALUE libsox_signal_alloc(VALUE signal) {
  sox_signalinfo_t *c_signal = ALLOC(sox_signalinfo_t);

  memset(c_signal, 0, sizeof(sox_signalinfo_t));
  return Data_Wrap_Struct(signal, 0, free, c_signal);
}

VALUE libsox_signal_dup(VALUE signal) {
  sox_signalinfo_t *c_signal;
  sox_signalinfo_t *c_dup_signal = ALLOC(sox_signalinfo_t);

  Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  c_dup_signal = c_signal; /* go deep */
  return Data_Wrap_Struct(LibSoXSignal, 0, free, c_dup_signal);
}

// LibSoXEncoding
VALUE libsox_encoding_bps(VALUE encoding) {
  sox_encodinginfo_t *c_enc;

  Data_Get_Struct(encoding, sox_encodinginfo_t, c_enc);
  return UINT2NUM(c_enc->bits_per_sample);
}

VALUE libsox_encoding_bps_set(VALUE encoding, VALUE bps) {
  sox_encodinginfo_t *c_enc;
  unsigned int val = NUM2UINT(bps);

  Data_Get_Struct(encoding, sox_encodinginfo_t, c_enc);
  c_enc->bits_per_sample = val;
  return bps;
}

VALUE libsox_encoding_encoding(VALUE encoding) {
  sox_encodinginfo_t *c_enc;

  Data_Get_Struct(encoding, sox_encodinginfo_t, c_enc);
  return UINT2NUM(c_enc->encoding);
}

VALUE libsox_encoding_encoding_set(VALUE encoding, VALUE codeing) {
  sox_encodinginfo_t *c_enc;
  unsigned int val = NUM2UINT(codeing);

  Data_Get_Struct(encoding, sox_encodinginfo_t, c_enc);
  c_enc->encoding = val;
  return encoding;
}

VALUE libsox_encoding_compression(VALUE encoding) {
  sox_encodinginfo_t *c_enc;

  Data_Get_Struct(encoding, sox_encodinginfo_t, c_enc);
  return DBL2NUM(c_enc->compression);
}
VALUE libsox_encoding_alloc(VALUE klass) {
  sox_encodinginfo_t *c_encoding = ALLOC(sox_encodinginfo_t);

  memset(c_encoding, 0, sizeof(sox_encodinginfo_t));
  return Data_Wrap_Struct(klass, 0, free, c_encoding);
}

// LibSoXFormat
static void libsox_format_close(void *ptr) {
  sox_close(ptr);
}

VALUE libsox_format_init(VALUE class) {
  int i = sox_format_init();
  return INT2NUM(i);
}

VALUE libsox_format_quit(VALUE class) {
  printf("quit format");
  sox_format_quit();
  return Qnil;
}

VALUE libsox_format_signal(VALUE format) {
  sox_format_t     *c_format;

  Data_Get_Struct(format, sox_format_t, c_format);
  return Data_Wrap_Struct(LibSoXSignal, 0, 0, &c_format->signal);
}

VALUE libsox_format_encoding(VALUE format) {
  sox_format_t *c_format;

  Data_Get_Struct(format, sox_format_t, c_format);
  return Data_Wrap_Struct(LibSoXEncoding, 0, 0, &c_format->encoding);
}

VALUE libsox_format_read(VALUE format, VALUE buffer, VALUE length) {
  sox_format_t *c_format;
  sox_sample_t *c_buffer;
  int c_length;

  Data_Get_Struct(format, sox_format_t, c_format);
  c_buffer = (sox_sample_t *)StringValuePtr(buffer);
  c_length = NUM2INT(length);
  return INT2NUM(sox_read(c_format, c_buffer, c_length));
}

VALUE libsox_format_write(VALUE format, VALUE buffer, VALUE length) {
  sox_format_t *c_format;
  sox_sample_t *c_buffer;
  int write_len = NUM2INT(length);

  Data_Get_Struct(format, sox_format_t, c_format);
  c_buffer = (sox_sample_t *)StringValuePtr(buffer);
  return INT2NUM(sox_write(c_format, c_buffer, write_len));
}

VALUE libsox_format_seek(VALUE format, VALUE offset) {
  sox_format_t *c_format;

  Data_Get_Struct(format, sox_format_t, c_format);
  return INT2NUM(sox_seek(c_format, NUM2LONG(offset), SOX_SEEK_SET));
}

// LibSoX
VALUE libsox_open_read(int argc, VALUE *argv, VALUE class) {
  VALUE path, signal, encoding, filetype;
  sox_signalinfo_t   *c_signal   = NULL;
  sox_encodinginfo_t *c_encoding = NULL;
  sox_format_t       *c_format;

  rb_scan_args(argc, argv, "13", &path, &signal, &encoding, &filetype);
  if (!NIL_P(signal)) Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  if (!NIL_P(encoding)) Data_Get_Struct(encoding, sox_encodinginfo_t, c_encoding);
  c_format = sox_open_read(StringValuePtr(path), c_signal, c_encoding, filetype == Qnil ? NULL : StringValuePtr(filetype));
  return Data_Wrap_Struct(LibSoXFormat, 0, libsox_format_close, c_format);
}

sox_bool libsox_overwrite_callback(const char *filename) {
  return sox_false;
}

VALUE libsox_open_write(int argc, VALUE *argv, VALUE class) {
  VALUE path, signal, encoding, filetype, oob;
  sox_signalinfo_t *c_signal = NULL;
  sox_encodinginfo_t *c_encoding = NULL;
  sox_oob_t *c_oob = NULL;
  sox_format_t *c_format;

  rb_scan_args(argc, argv, "14", &path, &signal, &encoding, &filetype, &oob);
  if (signal != Qnil) Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
  if (encoding != Qnil) Data_Get_Struct(encoding, sox_encodinginfo_t, c_encoding);
  if (oob != Qnil) Data_Get_Struct(oob, sox_oob_t, c_oob);
  c_format = sox_open_write(StringValuePtr(path),
    c_signal,
    c_encoding,
    filetype == Qnil ? NULL : StringValuePtr(filetype),
    c_oob,
    rb_block_given_p() ? libsox_overwrite_callback : NULL);
  return Data_Wrap_Struct(LibSoXFormat, 0, 0, c_format);
}

VALUE libsox_find_format(VALUE class, VALUE filetype) {
  sox_format_handler_t const * find_format;

  find_format = sox_find_format(StringValuePtr(filetype), sox_false);
  if(find_format)
    return Data_Wrap_Struct(LibSoXFormatHandler, 0, 0, (void *) find_format);
  else
    return Qnil;
}

static void libsox_destroy(void *instance) {
  printf("quit all");
  sox_format_quit();
  sox_quit();
}

VALUE libsox_new(VALUE class) {
  if(library_instance == 0)
  {
    library_instance = Data_Wrap_Struct(LibSoX, NULL, libsox_destroy, 0);
    rb_obj_call_init(library_instance, 0, 0);
    sox_init();
    sox_format_init();
  }
  return library_instance;
}

void Init_ruby_libsox(void) {
  rb_define_global_const("SOX_SUCCESS", INT2NUM(0));
  rb_define_global_const("SOX_EOF", INT2NUM(-1));

  int encode = 0;
  rb_define_global_const("SOX_ENCODING_UNKNOWN", INT2NUM(encode++));   /**< encoding has not yet been determined */

  rb_define_global_const("SOX_ENCODING_SIGN2", INT2NUM(encode++));      /**< signed linear 2's comp: Mac */
  rb_define_global_const("SOX_ENCODING_UNSIGNED", INT2NUM(encode++));   /**< unsigned linear: Sound Blaster */
  rb_define_global_const("SOX_ENCODING_FLOAT", INT2NUM(encode++));      /**< floating point (binary format) */
  rb_define_global_const("SOX_ENCODING_FLOAT_TEXT", INT2NUM(encode++)); /**< floating point (text format) */
  rb_define_global_const("SOX_ENCODING_FLAC", INT2NUM(encode++));       /**< FLAC compression */
  rb_define_global_const("SOX_ENCODING_HCOM", INT2NUM(encode++));       /**< Mac FSSD files with Huffman compression */
  rb_define_global_const("SOX_ENCODING_WAVPACK", INT2NUM(encode++));    /**< WavPack with integer samples */
  rb_define_global_const("SOX_ENCODING_WAVPACKF", INT2NUM(encode++));   /**< WavPack with float samples */
  rb_define_global_const("SOX_ENCODING_ULAW", INT2NUM(encode++));       /**< u-law signed logs: US telephony, SPARC */
  rb_define_global_const("SOX_ENCODING_ALAW", INT2NUM(encode++));       /**< A-law signed logs: non-US telephony, Psion */
  rb_define_global_const("SOX_ENCODING_G721", INT2NUM(encode++));       /**< G.721 4-bit ADPCM */
  rb_define_global_const("SOX_ENCODING_G723", INT2NUM(encode++));       /**< G.723 3 or 5 bit ADPCM */
  rb_define_global_const("SOX_ENCODING_CL_ADPCM", INT2NUM(encode++));   /**< Creative Labs 8 --> 2,3,4 bit Compressed PCM */
  rb_define_global_const("SOX_ENCODING_CL_ADPCM16", INT2NUM(encode++)); /**< Creative Labs 16 --> 4 bit Compressed PCM */
  rb_define_global_const("SOX_ENCODING_MS_ADPCM", INT2NUM(encode++));   /**< Microsoft Compressed PCM */
  rb_define_global_const("SOX_ENCODING_IMA_ADPCM", INT2NUM(encode++));  /**< IMA Compressed PCM */
  rb_define_global_const("SOX_ENCODING_OKI_ADPCM", INT2NUM(encode++));  /**< Dialogic/OKI Compressed PCM */
  rb_define_global_const("SOX_ENCODING_DPCM", INT2NUM(encode++));       /**< Differential PCM: Fasttracker 2 (xi) */
  rb_define_global_const("SOX_ENCODING_DWVW", INT2NUM(encode++));       /**< Delta Width Variable Word */
  rb_define_global_const("SOX_ENCODING_DWVWN", INT2NUM(encode++));      /**< Delta Width Variable Word N-bit */
  rb_define_global_const("SOX_ENCODING_GSM", INT2NUM(encode++));        /**< GSM 6.10 33byte frame lossy compression */
  rb_define_global_const("SOX_ENCODING_MP3", INT2NUM(encode++));        /**< MP3 compression */
  rb_define_global_const("SOX_ENCODING_VORBIS", INT2NUM(encode++));     /**< Vorbis compression */
  rb_define_global_const("SOX_ENCODING_AMR_WB", INT2NUM(encode++));     /**< AMR-WB compression */
  rb_define_global_const("SOX_ENCODING_AMR_NB", INT2NUM(encode++));     /**< AMR-NB compression */
  rb_define_global_const("SOX_ENCODING_CVSD", INT2NUM(encode++));       /**< Continuously Variable Slope Delta modulation */
  rb_define_global_const("SOX_ENCODING_LPC10", INT2NUM(encode++));      /**< Linear Predictive Coding */
  rb_define_global_const("SOX_ENCODING_OPUS", INT2NUM(encode++));       /**< Opus compression */
  rb_define_global_const("SOX_ENCODINGS", INT2NUM(encode++));           /**< End of list marker */

  LibSoX = rb_define_class("LibSoX", rb_cObject);
  rb_define_singleton_method(LibSoX, "new", libsox_new, 0);
  rb_define_singleton_method(LibSoX, "open_read", libsox_open_read, -1);
  rb_define_singleton_method(LibSoX, "open_write", libsox_open_write, -1);
  rb_define_singleton_method(LibSoX, "find_format", libsox_find_format, 1);

  /*
  LibSoXEffectHandler = rb_define_class("LibSoXEffectHandler", rb_cObject);
  rb_define_singleton_method(LibSoX, "new", libsox_effects_handler_new, 0);
  */

  LibSoXFormat = rb_define_class("LibSoXFormat", rb_cObject);
  rb_define_singleton_method(LibSoX, "init", libsox_format_init, 0);
  rb_define_singleton_method(LibSoX, "quit", libsox_format_quit, 0);
  rb_define_method(LibSoXFormat, "signal", libsox_format_signal, 0);
  rb_define_method(LibSoXFormat, "encoding", libsox_format_encoding, 0);
  rb_define_method(LibSoXFormat, "read", libsox_format_read, 2);
  rb_define_method(LibSoXFormat, "write", libsox_format_write, 2);
  rb_define_method(LibSoXFormat, "seek", libsox_format_seek, 1);

  LibSoXEffectsChain  = rb_define_class("LibSoXEffectsChain", rb_cObject);
  rb_define_singleton_method(LibSoXEffectsChain, "new", libsox_effects_chain_new, 2);
  rb_define_method(LibSoXEffectsChain, "add_effect", libsox_effects_chain_add_effect, 3);
  rb_define_method(LibSoXEffectsChain, "flow", libsox_effects_chain_flow_effects, 0);

  LibSoXSignal = rb_define_class("LibSoXSignal", rb_cObject);
  rb_define_alloc_func(LibSoXSignal, libsox_signal_alloc);
  rb_define_method(LibSoXSignal, "dup", libsox_signal_dup, 0);
  rb_define_method(LibSoXSignal, "precision", libsox_signal_precision, 0);
  rb_define_method(LibSoXSignal, "precision=", libsox_signal_precision_set, 1);
  rb_define_method(LibSoXSignal, "channels", libsox_signal_channels, 0);
  rb_define_method(LibSoXSignal, "channels=", libsox_signal_channels_set, 1);
  rb_define_method(LibSoXSignal, "length", libsox_signal_length, 0);
  rb_define_method(LibSoXSignal, "length=", libsox_signal_length_set, 1);
  rb_define_method(LibSoXSignal, "rate", libsox_signal_rate, 0);
  rb_define_method(LibSoXSignal, "rate=", libsox_signal_rate_set,1);

  LibSoXEncoding = rb_define_class("LibSoXEncoding", rb_cObject);
  rb_define_alloc_func(LibSoXEncoding, libsox_signal_alloc);
  rb_define_method(LibSoXEncoding, "encoding", libsox_encoding_encoding, 0);
  rb_define_method(LibSoXEncoding, "encoding=", libsox_encoding_encoding_set, 1);
  rb_define_method(LibSoXEncoding, "bps", libsox_encoding_bps, 0);
  rb_define_method(LibSoXEncoding, "bps=", libsox_encoding_bps, 1);
  rb_define_method(LibSoXEncoding, "compression", libsox_encoding_compression, 0);

  LibSoXEffect = rb_define_class("LibSoXEffect", rb_cObject);
  rb_define_singleton_method(LibSoXEffect, "new", libsox_effect_new, 1);
  rb_define_method(LibSoXEffect, "options", libsox_effect_options, -2);

  LibSoXFormatHandler = rb_define_class("LibSoXFormatHander", rb_cObject);
  /*
  rb_define_method(LibSoX, "chain",        libsox_effectschain, 2);
  rb_define_method(LibSoX, "buffer_size",  libsox_get_bufsize,  0);
  rb_define_method(LibSoX, "buffer_size=", libsox_set_bufsize,  1);

  rb_define_method(LibSoXFormat, "filename", libsoxformat_filename, 0);


  rb_define_method(LibSoXEffectsChain, "add",  libsoxeffectschain_add,  -1);
  rb_define_method(LibSoXEffectsChain, "flow", libsoxeffectschain_flow, -1);
  */
}

/*


VALUE libsox_set_bufsize(VALUE self, VALUE bufsize) {
  sox_globals.bufsiz = FIX2INT(bufsize);

  return Qtrue;
}

VALUE rsox_get_bufsize(VALUE self) {
  return INT2FIX(sox_globals.bufsiz);
}






VALUE rsoxformat_filename(VALUE self) {
  sox_format_t *c_format;

  Data_Get_Struct(self, sox_format_t, c_format);

  return rb_str_new2(c_format->filename);
}


typedef struct {
  VALUE block;
  ID func;
} rsox_block_with_id_t;

static int rsox_rubyblock_flow(sox_effect_t *effect, sox_sample_t const *ibuf, sox_sample_t *obuf, size_t *isamp, size_t *osamp) {
  // size_t i;
  rsox_block_with_id_t *param = (rsox_block_with_id_t *)effect->priv;
  VALUE buffer = Data_Wrap_Struct(LibSoxBuffer, 0, 0, (void *)ibuf);
  rb_iv_set(buffer, "@length", INT2NUM(*isamp));

  if (*isamp > 0)
    rb_funcall(param->block, param->func, 1, buffer);

  *osamp = 0;

  return SOX_SUCCESS;
}

static sox_effect_handler_t const *rsox_rubyblock_handler(void) {
  static sox_effect_handler_t handler = {
    "block", NULL, SOX_EFF_MCHAN, NULL, NULL, rsox_rubyblock_flow, NULL, NULL, NULL, sizeof(rsox_block_with_id_t)
  };

  return &handler;
}

VALUE rsoxeffectschain_add(int argc, VALUE *argv, VALUE self) {
  sox_effects_chain_t *c_chain;
  sox_effect_t *c_effect;
  sox_format_t *c_input, *c_output, *c_tmp_format;
  VALUE name, options, tmp;
  // VALUE input, output;
  sox_effect_handler_t const *c_handler;
  char *c_options[10], *c_name;
  int i, j;
  // int t;
  rsox_block_with_id_t *block_param;

  rb_scan_args(argc, argv, "1*", &name, &options);

  c_name = StringValuePtr(name);

  if (strncmp(c_name, "block", 5) == 0) {
    if (!rb_block_given_p())
      rb_raise(rb_eArgError, "no block given");

    c_handler = rsox_rubyblock_handler();
    c_effect = sox_create_effect(c_handler);

    block_param  = (rsox_block_with_id_t *)c_effect->priv;
    block_param->block = rb_block_proc();
    block_param->func  = rb_intern("call");
  } else {
    c_handler = sox_find_effect(StringValuePtr(name));
    if (c_handler == NULL)
      rb_raise(rb_eArgError, "no such effect: %s", StringValuePtr(name));
    c_effect = sox_create_effect(c_handler);

    for (i = j = 0; i < RARRAY_LEN(options); i++) {
      if (TYPE(RARRAY_PTR(options)[i]) == T_DATA) {
        Data_Get_Struct(RARRAY_PTR(options)[i], sox_format_t, c_tmp_format);
        c_options[j++] = (char *)c_tmp_format;
      } else {
        tmp = rb_check_string_type(RARRAY_PTR(options)[i]);
        c_options[j++] = NIL_P(tmp) ? NULL : RSTRING_PTR(tmp);
      }
    }

    i = sox_effect_options(c_effect, j, j > 0 ? c_options : NULL);
    if (i != SOX_SUCCESS)
      rb_raise(rb_eArgError, "wrong arguments (%d)", j);
  }

  Data_Get_Struct(self, sox_effects_chain_t, c_chain);
  Data_Get_Struct(rb_iv_get(self, "@input"),  sox_format_t, c_input);
  Data_Get_Struct(rb_iv_get(self, "@output"), sox_format_t, c_output);

  i = sox_add_effect(c_chain, c_effect, &c_input->signal, &c_output->signal);

  return INT2NUM(i);
}


VALUE rsoxbuffer_at(VALUE self, VALUE index) {
  sox_sample_t *c_buffer;

  if (index < rb_iv_get(self, "@length")) {
    Data_Get_Struct(rb_iv_get(self, "@buffer"), sox_sample_t, c_buffer);
    return INT2NUM(c_buffer[NUM2INT(index)]);
  }

  return Qnil;
}

VggALUE rsoxbuffer_buffer(VALUE self) {
  return rb_iv_get(self, "@buffer");
}

int rsoxeffectschain_flow_callback(sox_bool all_done, void *data) {
  return SOX_SUCCESS;
}

VALUE rsoxeffectschain_flow(int argc, VALUE *argv, VALUE self) {
  sox_effects_chain_t *c_chain;

  Data_Get_Struct(self, sox_effects_chain_t, c_chain);

  return INT2NUM(sox_flow_effects(c_chain, NULL, NULL));
}


  rb_define_method(LibSoxBuffer, "size",       rsoxbuffer_length,      0);
  rb_define_method(LibSoxBuffer, "buffer",     rsoxbuffer_buffer,      0);
  rb_define_method(LibSoxBuffer, "[]",         rsoxbuffer_at,          1);
  rb_define_method(LibSoxBuffer, "at",         rsoxbuffer_at,          1);



  */
