#!/bin/bash

################################################################################################################################################################

# @project        Library ▸ Astrodynamics
# @file           tools/ci/deploy-documentation.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

if [[ -z "${project_directory}" ]]; then
    echo "Missing [project_directory] variable."
    exit 1
fi

if [[ -z "${ci_doc_repo_ref}" ]]; then
    echo "Missing [ci_doc_repo_ref] variable."
    exit 1
fi

if [[ -z "${ci_doc_repo_name}" ]]; then
    echo "Missing [ci_doc_repo_name] variable."
    exit 1
fi

if [[ -z "${ci_doc_repo_user_name}" ]]; then
    echo "Missing [ci_doc_repo_user_name] variable."
    exit 1
fi

if [[ -z "${ci_doc_repo_user_email}" ]]; then
    echo "Missing [ci_doc_repo_user_email] variable."
    exit 1
fi

if [[ -z "${ci_doc_repo_token}" ]]; then
    echo "Missing [ci_doc_repo_token] variable."
    exit 1
fi

if [[ -z "${ci_build_number}" ]]; then
    echo "Missing [ci_build_number] variable."
    exit 1
fi

if [[ -z "${ci_commit}" ]]; then
    echo "Missing [ci_commit] variable."
    exit 1
fi

################################################################################################################################################################

# https://gist.github.com/vidavidorra/548ffbcdae99d752da02

mkdir -p "./gh-pages"

pushd "./gh-pages" > /dev/null

git clone -b gh-pages https://git@${ci_doc_repo_ref}

pushd "${ci_doc_repo_name}" > /dev/null

# Set the push default to simple i.e. push only the current branch.

git config --global push.default simple

# Pretend to be an user called Travis CI.

git config user.name ${ci_doc_repo_user_name}
git config user.email ${ci_doc_repo_user_email}

rm -rf ./*
mv .git git
rm -rf ./.*
mv git .git

cp "${project_directory}"/README.md .
cp -r "${project_directory}"/docs/* .

# Need to create a .nojekyll file to allow filenames starting with an underscore
# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
# Presumably this is only needed when the SHORT_NAMES option in Doxygen is set
# to NO, which it is by default. So creating the file just in case.

echo "" > .nojekyll

# Only upload if Doxygen successfully created the documentation.
# Check this by verifying that the html directory and the file html/index.html
# both exist. This is a good indication that Doxygen did its work.

if [ -d "html" ] && [ -f "html/index.html" ]; then

    # Add everything in this directory (the Doxygen code documentation) to the gh-pages branch.
    # GitHub is smart enough to know which files have changed and which files have
    # stayed the same and will only update the changed files.

    echo 'Adding documentation to the gh-pages branch...'

    git add --all

    # Commit the added files with a title and description containing the Travis CI
    # build number and the GitHub commit reference that issued this build.

    git commit -m "[feature] Deploy documentation to GitHub Pages" -m "Travis build: ${ci_build_number}" -m "Commit: ${ci_commit}"

    # Force push to the remote gh-pages branch.
    # The ouput is redirected to /dev/null to hide any sensitive credential data that might otherwise be exposed.

    echo 'Pushing documentation to remote...'

    git push --force "https://${ci_doc_repo_token}@${ci_doc_repo_ref}" > /dev/null 2>&1

else

    echo '' >&2
    echo '[Error] No documentation (html) files have been found!' >&2

    exit 1

fi

popd > /dev/null

################################################################################################################################################################
