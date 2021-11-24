Pod::Spec.new do |s|
    s.name = "swift-markdown"
    s.version = "0.0.1"
    s.summary = "Swift Markdown is a Swift package for parsing, building, editing, and analyzing Markdown documents."
    s.homepage = "https://github.com/apple/swift-markdown"
    s.license = { :type => "Apache", :file => "LICENSE.txt" }
    s.author = { "Apple" => "apple@apple.com" }
    s.source = { 
        :git => "https://github.com/vencewill/swift-markdown.git", 
        :tag => "pod-" + s.version.to_s,
        :submodules => true
    }
    s.ios.deployment_target = "9.0"
    s.tvos.deployment_target = "9.0"
    s.osx.deployment_target = "10.9"
    s.swift_version = '5.0'
    s.requires_arc = true
    s.module_name = "Markdown"
    s.source_files = [
        "Sources/Markdown/**/*.swift",
        "Sources/CAtomic/**/*.{h,c}",
        "swift-cmark/src/**/*.{h,c}",
        "swift-cmark/extensions/**/*.{h,c}",
    ]
    s.preserve_paths = [
        "README.md",
        "swift-cmark/src/include/module.modulemap",
        "swift-cmark/src/*.inc",
        "swift-cmark/COPYING",
        "swift-cmark/README.md",
    ]
    s.pod_target_xcconfig = {
        'SWIFT_INCLUDE_PATHS' => '$(SRCROOT)/swift-cmark/src/**',
        'SWIFT_ACTIVE_COMPILATION_CONDITIONS' => 'SWIFT_MARKDOWN_COCOA_PODS'
    }
    s.public_header_files = [
        "Sources/CAtomic/include/*.h",
        "swift-cmark/src/include/*.h",
        "swift-cmark/extensions/include/*.h",
    ]
end
