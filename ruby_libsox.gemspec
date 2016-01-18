# encoding: utf-8

Gem::Specification.new do |s|
  s.name    = 'ruby_libsox'
  s.version = '0.1.0'
  s.date    = Time.new.strftime('%Y-%m-%d')

  s.summary     = 'libSoX binding for Ruby'
  s.description = 'Ruby wrapper for libSoX, the Swiss Army knife of audio manipulation'

  s.authors  = [ 'Scott Moe' ]
  s.email    = 'scott.moe@cloudspace.com'
  s.homepage = 'https://github.com/zipi/ruby_libsox'

  s.rubyforge_project = nil
  s.has_rdoc = false

  s.extensions = [ "ext/ruby_libsox/extconf.rb" ]

  s.add_development_dependency "rake-compiler"
end
