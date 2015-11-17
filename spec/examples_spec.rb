require 'libsox'

# These examples are translated from the C-lang examples provided with the
# LibSoX source code.  They are not inteded to be the best ruby-way of working
# with the gem but did help me to TDD my way through development.
#

context 'example 0' do
  # with an input file apply effects to
  # decrease the volume and flang it
  it 'can open, change and save a file' do
    lib = LibSoX.new
    input = lib.open_read('./spec/fixtures/good_bad_ugly.wav')
    output = lib.open_write('./tmp/output.wav', input.signal)
    result = Chain.new(input, output).
      add_effect('input').
      add_effect('vol', '-3dB').
      add_effect('flanger').
      add_effect('output').
      flow
    expect(result).to eq SOX_SUCCESS
  end
end

context 'example 6' do
  # convert 2-channel 32-bit wav file
  # into a 1-channel 8-bit uLaw file
  it 'can convert files' do
    lib = LibSoX.new
    #lib.enable_debug
    input = lib.open_read './spec/fixtures/partita.wav'
    duration = input.signal.length / input.signal.rate / input.signal.channels
    new_length = ((duration * 8000) + 0.5).floor

    out_encoding = lib.new_encoding_info encoding: SOX_ENCODING_ULAW, bps: 8
    out_signal = lib.new_signal_info rate: 8000, channels: 1, precision: 8, length: new_length
    output = lib.open_write './tmp/ulaw.wav', out_signal, out_encoding
    interim_signal = input.signal.dup
    chain = Chain.new input, output
    chain.input_format = interim_signal
    chain.output_format = input.signal
    chain.add_effect 'input'
    chain.output_format = output.signal
    input.signal.rate != output.signal.rate && chain.add_effect('rate')
    input.signal.channels != output.signal.channels && chain.add_effect('channels')
    chain.add_effect 'dither'
    chain.add_effect 'output'

    expect(chain.flow).to eq SOX_SUCCESS
  end
end

context 'example 4' do
  # concatinate multiple input files
  # skip the trunkate file on error, but still raise an exception
  # ensure all files have the same signal characteristics
  # using sox_read and sox_write
  it 'can feed multiple files into the processing' do
    lib = LibSoX.new
    buffer_size = 2048
    samples = Buffer.new buffer_size
    signal = output = nil
    files = %w(./spec/fixtures/good_bad_ugly.wav ./spec/fixtures/make_my_day.wav)
    files.each_with_index do |f, i|
      input = lib.open_read(f)
      expect(input).to be_a LibSoXFormat
      if i == 0
        output = lib.open_write('./tmp/concat.wav', input.signal, input.encoding)
        signal = input.signal
      else
        raise "change in channels" if input.signal.channels != signal.channels
        raise "change in rate" if input.signal.rate != signal.rate
      end
      while (number_read = input.read(samples.raw_data, buffer_size)) > 0 do
        output.write(samples.raw_data, number_read)
      end
    end
  end
end

context 'example 2' do
  # open file, seek to start position,
  # read the period of sound in to a bufffer,
  # and then find the peak volume for each period sized, sample, in the buffer
  # print the reduced information as a histogram
  it 'can look at a buffer of samples' do
    lib = LibSoX.new
    # parameters in seconds
    start = 13.0
    period = 1.5
    block_period = 0.025
    input = lib.open_read './spec/fixtures/partita.wav'
    expect(input.signal.channels).to eq 2
    to_loc = start * input.signal.rate * input.signal.channels + 0.5
    to_loc -= to_loc % input.signal.channels
    expect(input.seek(to_loc)).to eq SOX_SUCCESS
    block_size = block_period * input.signal.rate * input.signal.channels + 0.5
    block_size -= block_size % input.signal.channels
    buffer = Buffer.new block_size
    graph = ''
    line = '=' * 35
    blocks = (period / block_period).ceil
    blocks.times do |block|
      count = input.read(buffer.raw_data, block_size)
      contents = buffer.contents
      samples = contents.map { |v| buffer.to_float(v).magnitude }
      rmax = lmax = 0.0
      buffer.contents.each_with_index do |v, i|
        i.even? && rmax < samples[i] && rmax = samples[i]
        i.odd? && lmax < samples[i] && lmax = samples[i]
      end
      point = start + block * block_period
      l = (1- lmax) * 35 + 0.5
      r = (1- rmax) * 35 + 0.5
      graph << sprintf("%8.3f %36s|%s\n", point, line[0,l], line[0,r])
    end
    # puts graph
    expect(graph.lines.count).to eq 60
  end
end

context 'example 3' do
  # on a system with a natural output channel like alsa
  # play a file handling changes to rate and channel count
  # with the rate and channels effects
  it 'plays a file with effects' do
    lib = LibSoX.new
    input = lib.open_read './spec/fixtures/make_my_day.wav'
    output = lib.open_write 'default', input.signal, nil, lib.play_filetype
    chain = Chain.new input, output
    interim_signal = input.signal.dup

    chain.input_format = interim_signal
    chain.output_format = input.signal
    chain.add_effect 'input'
    chain.add_effect 'trim', '1.2'

    chain.output_format = output.signal
    input.signal.rate != output.signal.rate && chain.add_effect('rate')
    input.signal.channels != output.signal.channels && chain.add_effect('channels')

    chain.add_effect 'output'
    expect(chain.flow).to eq SOX_SUCCESS
  end
end

context 'example 5' do
  # read file into buffer first
  # process file and save to memory buffer
  # Then write it out
  # just to show sox_mem_read and sox_mem_write in action
  it 'moves data through memory buffer' do
    lib = LibSoX.new
    MaxSamples = 2048
    input = lib.open_read './spec/fixtures/make_my_day.wav'
    samples = Buffer.new MaxSamples
    buffer_size = 123456
    buffer = Array.new(buffer_size, 0).pack('i*')
    output = lib.open_mem_write(buffer, buffer_size, input.signal, nil, "sox");
    raise "no support for POSIX fmemopen" unless output
    expect(output).to be_a LibSoXFormat
    while (count = input.read(samples.raw_data, MaxSamples)) > 0 do
      puts "got buffer #{count}"
      expect(output.write(samples.raw_data, count)).to eq count
    end

    memin = lib.open_mem_read(buffer, buffer.size);
    memout = lib.open_write('tmp/buffered.wav', memin.signal);
    while (count = memin.read(samples.raw_data, MaxSamples)) > 0 do
      expect(memout.write(sample.raw_data, count)).to eq count
    end
  end
end

=begin
context 'example 1' do
  # like example 0 
  # but with input_drain and output handlers
  # that happen to read and write from files
  # but could do something else
  it 'can use input and output handlers' do
    input = LibSoX.open_read('./spec/fixtures/good_bad_ugly.wav')
    output = LibSoX.open_write('./tmp/output.wav', input.signal)

    input_drain = ->(_effect, sample, size) do
      # insure sample size is multiple of number of channels
      size -= size % effect.out_signal.channels
      count = input.sox_read(sample, size)
      STDERR.puts("#{input.filename}: #{input.sox_error}") unless(count > 0 || input.sox_error > 0)
      count > 0 ? SOX_SUCCESS : SOX_ERROR
    end

    input_handler = LibSoXEffectHandler.new()
    chain = LibSoX.new_chain(input, output)
    chain.create_effect(input_handler)
    chain.add_effect('vol', '-3dB')
    chain.add_effect('flanger')
    chain.add_effect('output')
    expect(chain.flow).to eq 0
  end
end

=end
