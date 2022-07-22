source "https://rubygems.org"
ruby RUBY_VERSION

# to use GitHub Pages
gem "github-pages", group: :jekyll_plugins

# instead of uing "github-pages"
# gem 'jekyll'
gem "jekyll", "~> 3.9.0"
# gem 'jekyll-feed'
# gem 'jekyll-sitemap'
# gem 'jekyll-seo-tag'
# gem 'jemoji'
# gem 'jekyll-readme-index'
# gem 'kramdown-parser-gfm'

# If you have any plugins, put them here!
group :jekyll_plugins do
   gem "jekyll-feed"
   gem "jekyll-sitemap"
   # gem "jekyll-redirect-from"
   gem "jekyll-seo-tag"
   gem 'kramdown-parser-gfm'
end


# Windows and JRuby does not include zoneinfo files, so bundle the tzinfo-data gem
# and associated library.
platforms :mingw, :x64_mingw, :mswin, :jruby do
  gem "tzinfo", "~> 1.2"
  gem "tzinfo-data"
end

# Windows and JRuby does not include zoneinfo files, so bundle the tzinfo-data gem
# and associated library.
# gem 'tzinfo-data'
#install_if -> { RUBY_PLATFORM =~ %r!mingw|mswin|java! } do
#  gem "tzinfo", "~> 1.2"
#  gem "tzinfo-data"
#end

# Performance-booster for watching directories on Windows
#install_if -> { RUBY_PLATFORM =~ %r!mingw|mswin|java! } do
#  gem "wdm", "~> 0.1.1"
#end
#gem "wdm", "~> 0.1.1", :install_if => Gem.win_platform?

