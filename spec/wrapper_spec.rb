require 'libsox'

describe 'LibSoX' do
  before :all do
    @ls = LibSoX.new
  end

  describe 'interface class' do
    it 'does not create multiple instances' do
      two = LibSoX.new
      expect(two).to be @ls
    end

    it 'exposes the open_read and _write  methods' do
      lsmethods = LibSoX.methods
      expect(lsmethods.include?(:open_read)).to be true
      expect(lsmethods.include?(:open_write)).to be true
    end
  end

  describe 'handling input and output' do
    it 'can open an input file and understand it' do
      input = LibSoX.open_read('spec/fixtures/good_bad_ugly.wav')
      expect(input).to_not be_nil
      expect(input).to_not eq 0
      expect(input).to be_a LibSoXFormat
      expect(input.signal).to be_a LibSoXSignal
      expect(input.signal.rate).to eq 11025.0
      expect(input.signal.precision).to eq 8
      expect(input.signal.channels).to eq 1
      expect(input.signal.length).to eq 94976
    end

    it 'can open a file for output' do
      input = LibSoX.open_read('./spec/fixtures/good_bad_ugly.wav')
      output = LibSoX.open_write('./tmp/output.wav', input.signal)
      expect(output).to_not be_nil
      expect(output).to_not eq 0
      expect(output).to be_a LibSoXFormat
      expect(output.signal).to be_a LibSoXSignal
      expect(output.signal.rate).to eq input.signal.rate
      expect(output.signal.precision).to eq  input.signal.precision
      expect(output.signal.channels).to eq input.signal.channels
      expect(output.signal.length).to eq input.signal.length
    end
  end
end
