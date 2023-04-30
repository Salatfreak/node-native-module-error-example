const binding = require('./build/Release/poll.node')

exports.poll = async () => await new Promise((res, rej) => {
  binding.poll(err => (err === undefined) ? res() : rej(err))
})
