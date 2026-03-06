# Contributing to Windminer

Thank you for your interest in contributing! This guide will help you get started.

## Code of Conduct

Please be respectful and constructive in all interactions.

## How to Contribute

### Reporting Bugs

1. Check existing issues first
2. Use the bug report template
3. Include: OS, compiler version, steps to reproduce

### Suggesting Features

1. Open a discussion before creating an issue
2. Describe the use case clearly
3. Explain why it benefits the project

### Submitting Code

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Make your changes
4. Run tests (`make test`)
5. Commit with clear messages
6. Open a pull request

## Development Setup

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/windminer.git
cd windminer

# Create development branch
git checkout -b dev

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run tests
make test
```

## Coding Standards

### C++

- Follow C++17 standard
- Use `clang-format` with provided `.clang-format`
- Document all public APIs with Doxygen comments
- Keep functions under 50 lines when possible

### Rust

- Follow Rust 2021 edition
- Run `cargo fmt` before committing
- Use `clippy` for linting
- Document public modules with rustdoc

### ALN-Blockchain

- Follow ALN smart contract standards
- Include gas optimization comments
- Test on testnet before mainnet

## Testing Requirements

- All new features must include tests
- Maintain >80% code coverage
- CI must pass before merge

## Questions?

Open a discussion or contact maintainers at: contributors@ecotribute.org
```
