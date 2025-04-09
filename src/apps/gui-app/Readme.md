# nodejs
Installing nodejs

## Download and install nvm:
```bash
$ curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.2/install.sh | bash
```
## in lieu of restarting the shell
```bash
$ \. "$HOME/.nvm/nvm.sh"
```
## Download and install Node.js:
```bash
$ nvm install 22
```
## Verify the Node.js version:
```bash
$ node -v # Should print "v22.14.0".
$ nvm current # Should print "v22.14.0".
```
## Verify npm version:
```bash
$ npm -v # Should print "10.9.2".
```

# vehicle-app
Inside that directory, you can run several commands:

## Starts the development server.
```bash
  $ npm start
```
## Bundles the app into static files for production.
```bash
  $ npm run build
```
## Starts the test runner.
```bash
  npm test
```

Removes this tool and copies build dependencies, configuration files
and scripts into the app directory. If you do this, you can’t go back!
```bash
  $ npm run eject
```

We suggest that you begin by typing:

```bash
  $ cd vehicle-app
  $ npm start
```
