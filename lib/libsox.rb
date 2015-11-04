require 'ruby_libsox'

class Chain
  def initialize(input, output)
    @input = input
    @output = output
    @chain = LibSoXEffectsChain.new(@input, @output)
  end

  def open_read(filename)
    @input = LibSoX.open_read(filename)
    self
  end

  def open_write(filename)
    signal = @input && @input.signal
    @output = LibSoX.oepn_write(filename, signal)
    self
  end

  def add_effect(name, *args)
    effect = make_effect(name)
    if %w(input output).include? name
      effect.options(input(name) || output(name))
    else
      effect.options(*args)
    end
    @chain.add_effect(effect, @input.signal)
    self
  end

  def flow
    @chain.flow
  end

  private

  def make_effect(name)
    LibSoXEffect.new(name) || raise("Effect: #{name} not found")
  end

  def input(name)
    name == 'input' && @input
  end

  def output(name)
    name == 'output' && @output
  end
end

class EffectHandler

  def initialize(options)
    # values name, usage, flags(SOX_EFF)
    # functions getopts, start, flow, drain, stop, kill
    # priv_size

  end

  def start

  end

  def stop

  end

  def flow

  end

  def drain

  end

  def kill

  end

  def getopts

  end
end

class Buffer
  # manage an array of 32bit integers used a buffer of samples
  attr_accessor :raw_data
  PKA = 'i*'

  def initialize(size)
    values = Array.new(size, 0)
    @raw_data = values.pack(PKA)
  end

  def contents
    @raw_data.unpack(PKA)
  end

  def repack(values)
    @raw_data = values.pack(PKA)
  end

  def to_float(value)
    max = 0x7FFFFFFF
    value * (1.0 / (max + 1.0))
  end
end

module Extension
  def new_chain(input, output)
    Chain.new(input, output)
  end

  def new_signal_info(params)
    signal = LibSoXSignal.new
    params.each { |k, v| signal.send(k.to_s+'=', v) }
    signal
  end

  def new_encoding_info(params)
    encoding = LibSoXEncoding.new
    params.each { |k,v| encoding.send(k.to_s+'=', v) }
    encoding
  end

  def new_buffer(size = 1024)
    Buffer.new(size)
  end
end

LibSoX.extend Extension
