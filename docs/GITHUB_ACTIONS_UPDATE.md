# GitHub Actions Version Update

## Issue

All CI workflows were failing with the error:
```
Error: This request has been automatically failed because it uses a deprecated version of
`actions/upload-artifact: v3`. Learn more:
https://github.blog/changelog/2024-04-16-deprecation-notice-v3-of-the-artifact-actions/
```

## Root Cause

GitHub deprecated v3 of several actions and is now enforcing the use of v4+:
- `actions/checkout@v3` (deprecated)
- `actions/upload-artifact@v3` (deprecated)
- `codecov/codecov-action@v3` (deprecated)
- `actions/github-script@v6` (deprecated)

Starting in 2024, GitHub automatically fails workflows using these deprecated versions.

## Solution Applied

Updated all GitHub Actions to their latest stable versions:

| Action | Old Version | New Version |
|--------|-------------|-------------|
| actions/checkout | v3 | v4 |
| actions/upload-artifact | v3 | v4 |
| codecov/codecov-action | v3 | v4 |
| actions/github-script | v6 | v7 |

## Files Modified

1. **`.github/workflows/ci.yml`**
   - Updated checkout action (2 instances)
   - Updated upload-artifact action (2 instances)
   - Updated codecov-action (1 instance)

2. **`.github/workflows/cppcheck.yml`**
   - Updated checkout action (1 instance)
   - Updated upload-artifact action (1 instance)
   - Updated github-script action (1 instance)

3. **`.github/workflows/pre-commit.yml`**
   - Updated checkout action (1 instance)

## Changes Made

### actions/checkout@v4
**What's New:**
- Better support for sparse checkouts
- Improved performance for large repositories
- Better handling of Git LFS

**Breaking Changes:** None for basic usage

### actions/upload-artifact@v4
**What's New:**
- Significant performance improvements
- Better compression
- More efficient artifact storage
- Support for artifact retention policies

**Breaking Changes:**
- New artifact backend (transparent to users)
- Improved artifact naming and organization

### codecov/codecov-action@v4
**What's New:**
- Better integration with GitHub API
- Improved error reporting
- Faster upload speeds

**Breaking Changes:** None for basic usage

### actions/github-script@v7
**What's New:**
- Updated to use newer GitHub REST API
- Better TypeScript support
- Improved error handling

**Breaking Changes:** Minor API changes (not affecting our usage)

## Verification

After this update, all workflows should:
1. ✅ Start successfully without deprecation warnings
2. ✅ Execute all steps correctly
3. ✅ Upload artifacts successfully
4. ✅ Report code coverage (if applicable)
5. ✅ Comment on PRs (if applicable)

## Testing Locally

These changes only affect GitHub Actions workflows. Local development is unaffected.

To verify workflows work:
1. Push to the branch
2. Check GitHub Actions tab: https://github.com/psanodiya94/cpp-odbc-wrapper/actions
3. Verify all workflows show green checkmarks ✅

## References

- [GitHub Deprecation Notice](https://github.blog/changelog/2024-04-16-deprecation-notice-v3-of-the-artifact-actions/)
- [actions/checkout v4](https://github.com/actions/checkout/releases/tag/v4.0.0)
- [actions/upload-artifact v4](https://github.com/actions/upload-artifact/releases/tag/v4.0.0)
- [codecov/codecov-action v4](https://github.com/codecov/codecov-action/releases/tag/v4.0.0)
- [actions/github-script v7](https://github.com/actions/github-script/releases/tag/v7.0.0)

## Impact

- **Positive:** Workflows use modern, supported actions
- **Positive:** Better performance and reliability
- **Positive:** Future-proof for GitHub's platform
- **Neutral:** No changes to workflow logic or behavior
- **Neutral:** No impact on local development

## Commit

**Commit hash:** `a945e61`
**Message:** `fix(ci): update deprecated GitHub Actions to v4`

All workflows should now execute successfully! ✅
