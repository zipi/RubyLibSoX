require 'ruby_libsox'

context 'example 0' do
  # with an input file
  # decrease the volume and flang it with effects
  it 'can open, change and save a file' do
    input = LibSoX.open_read('./spec/fixtures/good_bad_ugly.wav')
    output = LibSoX.open_write('./tmp/output.wav', input.signal)
    result = LibSoX.new_chain(input, output).
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
    out_encoding = LibSoX.new_encoding_info encoding: SOX_ENCODING_ULAW, bps: 8
    out_signal = LibSoX.new_signal_info rate: 8000, channels: 1
    input = LibSoX.open_read './spec/fixtures/make_my_day.wav'
    output = LibSoX.open_write './tmp/day.wav', out_signal, out_encoding
    chain = LibSoX.new_chain input, output
    chain.add_effect 'input'
    input.signal.rate != output.signal.rate && chain.add_effect('rate')
    input.signal.channels != output.signal.channels && chain.add_effect('channels')
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
    files = %w(./spec/fixtures/good_bad_ugly.wav ./spec/fixtures/make_my_day.wav)
    buffer_size = 2048
    signal = nil
    output = nil
    samples = LibSoX.new_buffer size: buffer_size
    files.each_with_index do |f, i|
      input = LibSoX.open_read(f)
      expect(input).to be_a LibSoXFormat
      if i == 0
        output = LibSoX.open_write('./tmp/concat.wav', input.signal, input.encoding)
        signal = input.signal
      else
        raise "change in channels" if input.signal.channels != signal.channels
        raise "change in rate" if input.signal.rate != signal.rate
      end
      while (number_read = input.read(samples.raw_data, buffer_size)) > 0 do
        output.write(samples.raw_data, number_read)
      end
      expect(input.close).to eq SOX_SUCCESS
    end
    expect(output.close).to eq SOX_SUCCESS
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

context 'example 2' do
  start = 60
  period = 5
  sample = 0.1

  # open file, seek to start position,
  # read the period of sound in to a bufffer,
  # and then find the peak volume for each sample sized, sample, in the buffer
  # print the reduced information as a histogram
end

context 'example 3' do
  # on a system with a natural output channel like alsa
  # play a file handling changes to rate and channel count
  # with the rate and channels effects
end

context 'example 5' do
  # read file into buffer first
  # process file and save to memory buffer
  # Then write it out
  # just to show sox_mem_read and sox_mem_write in action
end

=end
